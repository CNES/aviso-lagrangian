import hashlib
import os
import pathlib
import sys
import tempfile
import threading
import urllib.request
import zipfile

#: Lock to avoid concurrent downloads
DOWNLOAD_LOCK = threading.RLock()


class TestData:
    """
    Utility class for managing test data downloads.
    """
    #: Dictionary of files to download with their expected SHA256 checksums.
    FILES: dict[str, str] = {
        'dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc':
        '05edd6958638cde0ef97191cb2a46399273d5a5051efe443c5c579ece3e18dd7',
        'dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc':
        'f3ec6e4c521ae3b3feccfbb9964f60f99039ca98c4dbff4394a442be0ff86def',
        'dt_upd_global_merged_madt_uv_20100113_20100113_20110329.nc':
        '7acf23ac5a7311bd76d4cd5504c65e35c30a09a7880d9079a7f29a28f4237335',
        'dt_upd_global_merged_madt_uv_20100120_20100120_20110329.nc':
        '8f69c39bf245749db2c48d89501aee35635edb6cd2d9cffd510160dfc66a6d39',
        'dt_upd_global_merged_madt_uv_20100127_20100127_20110329.nc':
        '673504b0e2699a141a3df847a5c8badda2bd9c6b29c98e95ee75011258973f15',
        'dt_upd_global_merged_madt_uv_20100203_20100203_20110329.nc':
        '2c1c5cfb81481433d75292cc39a8d8a3a08b995413a999bb70b86a0d9a6abe4f',
        'dt_upd_global_merged_madt_uv_20100210_20100210_20110329.nc':
        '5e22d21c7a5d3f29225cc1a17af7be826cc37c92252fe992bf76a9a444c8140d',
        'dt_upd_global_merged_madt_uv_20100217_20100217_20110329.nc':
        'a072cd524e39d03d7900d2a39db3f1ed7f0ee1a02ee41d08c4d721e020553921',
        'dt_upd_global_merged_madt_uv_20100224_20100224_20110329.nc':
        'acef1760970ce139a509bf92b1c23a30478f868f8daf4e8b50cbb0fed9f4c2bd',
        'dt_upd_global_merged_madt_uv_20100303_20100303_20110329.nc':
        'ea2c0309ac7d2fda60539a903b86a92e5a590550580c120965a459d30b04f082',
        'dt_upd_global_merged_madt_uv_20100310_20100310_20110329.nc':
        '0bf8a32c648e0cb9f8414faa136aff461a0f94c3cb1df4828b49c3dac45ddd34',
        'dt_upd_global_merged_madt_uv_20100317_20100317_20110329.nc':
        '5993233e0690cc86f03eff4653cbb15ac82045d0a04b84417e5b841616a2d830',
        'dt_upd_global_merged_madt_uv_20100324_20100324_20110329.nc':
        '092316e853a154d182e5f9f439593e6333ae17f652038f5ccdc73c1cdf0be23e',
        'dt_upd_global_merged_madt_uv_20100331_20100331_20110329.nc':
        'c0e717977296add28ce297f19c2110bcd1499793cbdff36c613658715c8ad027',
        'dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc':
        'eb58f62c1a4b9a7a5f9019c8cce85bcb91cc581afd49f16ff9d1e968d0f1d4a6',
        'fsle.nc':
        '39894437cc1d3db788e359b1257d60cd164b96b3d346af0bc2c07a27ae9378d1'
    }

    #: URL to download the data from
    URL = 'https://github.com/CNES/aviso-lagrangian/wiki/data.zip'

    #: The directory containing this file.
    ROOT = pathlib.Path(__file__).absolute().parent

    def __init__(self):
        self.prefix = self.ROOT / 'data'
        self.prefix.mkdir(parents=True, exist_ok=True)
        with DOWNLOAD_LOCK:
            if not self.check():
                temp = pathlib.Path(tempfile.NamedTemporaryFile().name)
                try:
                    self.download(temp)
                    self.extract(temp)
                finally:
                    temp.unlink()

    def extract(self, archive: pathlib.Path) -> None:
        """
        Extraction of the downloaded archive.

        Args:
            archive: The path to the downloaded zip file.
        """
        zip_file = zipfile.ZipFile(archive)
        for name in zip_file.namelist():
            info = zip_file.getinfo(name)
            deflated = self.prefix / name
            with open(deflated, 'wb') as stream:
                stream.write(zip_file.read(info))

    def check(self) -> bool:
        """
        Verify data integrity
        """
        for item, checksum in self.FILES.items():
            path = self.prefix / item
            if not path.exists() or self.sha256sum(path) != checksum:
                return False
        return True

    @classmethod
    def download(cls, archive: pathlib.Path) -> None:
        """Download data from the specified URL.

        Args:
            archive: The path where the downloaded zip file will be saved.
        """
        # Only show progress if running interactively (not in CI)
        hook = (TestData.progress_hook
                if sys.stderr.isatty() and not os.getenv('CI') else None)
        urllib.request.urlretrieve(cls.URL, archive, reporthook=hook)
        sys.stderr.write('\n')  # Add newline after progress bar

    @staticmethod
    def sha256sum(path: pathlib.Path) -> str:
        """Computes the SHA256 hash for a file

        Args:
            path: The path to the file to hash.

        Returns:
            The SHA256 hash of the file as a hexadecimal string.
        """
        sha256 = hashlib.sha256()
        with open(path, 'rb') as stream:
            for block in iter(lambda: stream.read(65536), b''):
                sha256.update(block)
        return sha256.hexdigest()

    @staticmethod
    def progress_hook(count: int, block_size: int, total_size: int) -> None:
        """Progress hook for urllib to show download progress.

        Args:
            count: Number of blocks transferred so far.
            block_size: Size of each block in bytes.
            total_size: Total size of the file being downloaded in bytes.
        """
        downloaded = count * block_size
        percent = min(100, (downloaded * 100) // total_size)

        # Show progress bar with percentage and size info
        bar_length = 50
        filled_length = int(bar_length * percent // 100)
        bar = '█' * filled_length + '░' * (bar_length - filled_length)

        # Format file sizes
        def format_bytes(bytes_val):
            for unit in ['B', 'KB', 'MB', 'GB']:
                if bytes_val < 1024.0:
                    return f'{bytes_val:.1f} {unit}'
                bytes_val /= 1024.0
            return f'{bytes_val:.1f} TB'

        downloaded_str = format_bytes(downloaded)
        total_str = format_bytes(total_size)

        # Print progress on same line using carriage return to stderr for
        # pytest visibility
        sys.stderr.write(f'\rDownloading test dataset: {bar} {percent}% '
                         f'({downloaded_str}/{total_str})')
        sys.stderr.flush()

    @classmethod
    def folder(cls) -> pathlib.Path:
        """Returns the folder where the data is stored."""
        return cls.ROOT / 'data'
