"""
This script is the entry point for unit tests.
"""
# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
import datetime
import hashlib
try:
    import urllib.request as urllib
except ImportError:
    import urllib as urllib
import os
import tempfile
import unittest
import zipfile
import lagrangian


class DownloadData(object):
    """
    Utility class for downloading test data to the website
    """
    FILES = {
        "dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc":
            "05edd6958638cde0ef97191cb2a46399273d5a5051efe443c5c579ece3e18dd7",
        "dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc":
            "f3ec6e4c521ae3b3feccfbb9964f60f99039ca98c4dbff4394a442be0ff86def",
        "dt_upd_global_merged_madt_uv_20100113_20100113_20110329.nc":
            "7acf23ac5a7311bd76d4cd5504c65e35c30a09a7880d9079a7f29a28f4237335",
        "dt_upd_global_merged_madt_uv_20100120_20100120_20110329.nc":
            "8f69c39bf245749db2c48d89501aee35635edb6cd2d9cffd510160dfc66a6d39",
        "dt_upd_global_merged_madt_uv_20100127_20100127_20110329.nc":
            "673504b0e2699a141a3df847a5c8badda2bd9c6b29c98e95ee75011258973f15",
        "dt_upd_global_merged_madt_uv_20100203_20100203_20110329.nc":
            "2c1c5cfb81481433d75292cc39a8d8a3a08b995413a999bb70b86a0d9a6abe4f",
        "dt_upd_global_merged_madt_uv_20100210_20100210_20110329.nc":
            "5e22d21c7a5d3f29225cc1a17af7be826cc37c92252fe992bf76a9a444c8140d",
        "dt_upd_global_merged_madt_uv_20100217_20100217_20110329.nc":
            "a072cd524e39d03d7900d2a39db3f1ed7f0ee1a02ee41d08c4d721e020553921",
        "dt_upd_global_merged_madt_uv_20100224_20100224_20110329.nc":
            "acef1760970ce139a509bf92b1c23a30478f868f8daf4e8b50cbb0fed9f4c2bd",
        "dt_upd_global_merged_madt_uv_20100303_20100303_20110329.nc":
            "ea2c0309ac7d2fda60539a903b86a92e5a590550580c120965a459d30b04f082",
        "dt_upd_global_merged_madt_uv_20100310_20100310_20110329.nc":
            "0bf8a32c648e0cb9f8414faa136aff461a0f94c3cb1df4828b49c3dac45ddd34",
        "dt_upd_global_merged_madt_uv_20100317_20100317_20110329.nc":
            "5993233e0690cc86f03eff4653cbb15ac82045d0a04b84417e5b841616a2d830",
        "dt_upd_global_merged_madt_uv_20100324_20100324_20110329.nc":
            "092316e853a154d182e5f9f439593e6333ae17f652038f5ccdc73c1cdf0be23e",
        "dt_upd_global_merged_madt_uv_20100331_20100331_20110329.nc":
            "c0e717977296add28ce297f19c2110bcd1499793cbdff36c613658715c8ad027",
        "dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc":
            "eb58f62c1a4b9a7a5f9019c8cce85bcb91cc581afd49f16ff9d1e968d0f1d4a6"
    }

    def __init__(self):
        self.prefix = os.path.join(os.path.dirname(__file__), "data")
        while not self.check():
            temp = tempfile.NamedTemporaryFile()
            self.download(temp)
            temp.flush()
            self.extract(temp.name)

    def extract(self, name):
        """
        Extraction of the downloaded archive.
        """
        zip_file = zipfile.ZipFile(name)
        for name in zip_file.namelist():
            info = zip_file.getinfo(name)
            deflated = os.path.join(self.prefix, name)
            with open(deflated, "wb") as stream:
                stream.write(zip_file.read(info))

    def check(self):
        """
        Verify data integrity
        """
        for item in self.FILES:
            path = os.path.join(self.prefix, item)
            if not os.path.exists(path) or \
                    self.sha256sum(path) != self.FILES[item]:
                return False
        return True

    @staticmethod
    def download(stream):
        """
        Download data from bitbucket
        """
        url = "https://bitbucket.org/cnes_aviso/lagrangian/downloads/data.zip"
        print("downloading https://bitbucket.org/cnes_aviso/lagrangian/downloads/data.zip")
        response = urllib.urlopen(url)

        while True:
            data = response.read(65536)
            if not data:
                break
            stream.write(data)

    @staticmethod
    def sha256sum(path):
        """
        Computes the SHA256 hash for a file
        """
        sha256 = hashlib.sha256()
        with open(path, 'rb') as stream:
            for block in iter(lambda: stream.read(65536), b''):
                sha256.update(block)
        return sha256.hexdigest()

DownloadData()
