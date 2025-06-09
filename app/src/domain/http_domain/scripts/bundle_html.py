#!/usr/bin/env python3

import re
import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--html", required=True, help="Path to main HTML file")
parser.add_argument("--output", required=True, help="Output path for bundled file")
parser.add_argument("--base", default=None, help="Base directory for relative includes (default: directory of HTML file)")

args = parser.parse_args()
html_path = Path(args.html)
base_dir = Path(args.base) if args.base else html_path.parent
output_path = Path(args.output)

html = html_path.read_text()

# Inline CSS <link>
html = re.sub(
    r'<link\s+rel="stylesheet"\s+href="([^"]+)"\s*/>',
    lambda m: f"<style>\n{(base_dir / m.group(1)).read_text()}\n</style>",
    html
)

# Inline JS <script>
html = re.sub(
    r'<script\s+src="([^"]+)"\s*/>',
    lambda m: f"<script>\n{(base_dir / m.group(1)).read_text()}\n</script>",
    html
)

output_path.parent.mkdir(parents=True, exist_ok=True)
output_path.write_text(html)
print(f"Bundled HTML written to {output_path}")
