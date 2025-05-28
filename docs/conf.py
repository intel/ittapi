#
# Copyright (C) 2025 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------

project = 'ITT/JIT APIs Documentation'
copyright = '2025 Intel Corporation'
author = 'Intel Corporation'

# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx_rtd_theme',        # ReadTheDocs theme
    'sphinx.ext.githubpages',  # Support for GitHub Pages
    'sphinx.ext.ifconfig',     # Conditional inclusion of content
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------

import sphinx_rtd_theme

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'style_external_links': True,
}

html_baseurl = 'https://intel.github.io/ittapi/'
