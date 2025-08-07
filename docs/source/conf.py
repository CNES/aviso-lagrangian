# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import importlib.metadata
import os
import pathlib
import sysconfig
import sys

#: The directory containing this file.
HERE = pathlib.Path(__file__).absolute().parent


def build_dirname(extname=None):
    """Returns the name of the build directory."""
    return pathlib.Path(
        HERE.parent.parent, 'build',
        f'lib.{sysconfig.get_platform()}-{sys.implementation.cache_tag}')


def push_front_syspath():
    """Add the build directory to the front of sys.path."""
    if HERE.parent.parent.joinpath('setup.py').exists():
        # We are in the root directory of the development tree
        sys.path.insert(0, str(build_dirname().resolve()))


# Insert the project root dir as the first element in the PYTHONPATH.
push_front_syspath()

try:
    release = importlib.metadata.version('lagrangian')
except importlib.metadata.PackageNotFoundError:
    try:
        import lagrangian
        release = lagrangian.__version__
        release = '.'.join(release.split('.')[:3])
    except ImportError:
        release = '0.0.0'
version = '.'.join(release.split('.')[:2])


# -- Project information -----------------------------------------------------

project = 'lagrangian'
copyright = '2025, CLS/LOCEAN'
author = 'CLS/LOCEAN'

# The full version, including alpha/beta/rc tags
release = version

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.autosummary',
    'sphinx.ext.mathjax',
    'sphinx.ext.napoleon',
    'sphinx.ext.viewcode',
]

# Autodoc configuration for better handling of pybind11 modules
autodoc_default_options = {
    'members': True,
    'member-order': 'bysource',
    'special-members': '__init__',
    'undoc-members': True,
    'exclude-members': '__weakref__'
}

# Mock imports for submodules that Sphinx has trouble with
autodoc_mock_imports = []

autosummary_generate = True
numpydoc_class_members_toctree = True
numpydoc_show_class_members = False

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_book_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

intersphinx_mapping = {
    'python': ('http://docs.python.org/3/', None),
    'numpy': ('https://docs.scipy.org/doc/numpy/', None),
}
