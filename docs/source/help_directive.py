"""Sphinx extension to display the help of a Python module.

The target Python module must define a function called `usage` that prints the
help message if the `--help` option is passed to the script.
"""
import contextlib
import importlib
import io
import subprocess
import sys
from collections.abc import Callable
from typing import Any, ClassVar

from docutils import nodes
from docutils.parsers.rst import Directive


class HelpDirective(Directive):
    """Directive to display the help of a Python module."""
    has_content = True
    option_spec: ClassVar[dict[str, Callable[[str], Any]] | None] = {
        'program_name': lambda x: x,
    }

    def run(self):
        if not self.content:
            raise ValueError('The directive content is required')
        pymod_name = self.content[0]
        program_name = self.options.get('program_name', pymod_name)
        argv = sys.argv
        sys.argv = [program_name, '--help']
        try:
            pymod = importlib.import_module(pymod_name)
            buffer = io.StringIO()
            usage = pymod.usage
            with contextlib.redirect_stdout(buffer):
                try:
                    usage()
                except SystemExit:
                    pass
            output = buffer.getvalue()
        except subprocess.CalledProcessError as e:
            output = f"Error while retrieving help: {e}"
        finally:
            sys.argv = argv

        literal = nodes.literal_block(output, output)
        literal['language'] = 'console'
        return [literal]


def setup(app):
    """Setup the Sphinx extension."""
    app.add_directive('help_directive', HelpDirective)
