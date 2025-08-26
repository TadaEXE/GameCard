#!/usr/bin/env python3
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
from pathlib import Path

def check_arduino_cli():
    """Return path to arduino-cli, installing if needed."""
    cli = shutil.which("arduino-cli")
    if cli:
        print(f"[OK] Found arduino-cli at {cli}")
        return cli

    print("[INFO] arduino-cli not found, installing latest release...")
    url = "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz"
    tmpdir = tempfile.mkdtemp()
    archive_path = os.path.join(tmpdir, "arduino-cli.tar.gz")

    # Download
    print(f"[INFO] Downloading {url}")
    urllib.request.urlretrieve(url, archive_path)

    # Extract
    with tarfile.open(archive_path, "r:gz") as tar:
        tar.extractall(tmpdir)

    # Move binary to ~/.local/bin (preferred user path)
    bin_dir = Path.home() / ".local" / "bin"
    bin_dir.mkdir(parents=True, exist_ok=True)
    extracted = Path(tmpdir) / "arduino-cli"
    shutil.move(str(extracted), str(bin_dir / "arduino-cli"))

    print(f"[OK] Installed arduino-cli to {bin_dir}")
    return str(bin_dir / "arduino-cli")

def init_config(cli_path, steps):
    """Create target_nucleo_l053r8/config and run config init."""
    config_dir = Path("target_nucleo_l053r8/.config")
    config_dir.mkdir(parents=True, exist_ok=True)
    print(f"[INFO] Initializing arduino-cli config in {config_dir}")

    def run_cmd(*args):
        subprocess.run(
            [cli_path, "--config-dir", str(config_dir), *args],
            check=True,
        )

    for step in steps:
        try:
            run_cmd(*step)
        except subprocess.CalledProcessError as e:
            print(f"[ERROR] Command failed: {e}", file=sys.stderr)
            continue
        except Exception as e:
            print(f"[ERROR] {e}", file=sys.stderr)
            sys.exit(1)
    
arduino_libs = [
    ["lib", "install", "Adafruit SH110X"]
]

setup_steps_l053r8 = [
    ["config", "init"],
    ["core", "update-index"],
    ["core", "install", "STMicroelectronics:stm32", "--additional-urls", "https://raw.githubusercontent.com/stm32duino/BoardManagerFiles/main/package_stmicroelectronics_index.json"],
    ["board", "listall"],
    *arduino_libs,
]

def main():
    cli_path = check_arduino_cli()
    init_config(cli_path, setup_steps_l053r8)
    print("[DONE] Setup complete.")

if __name__ == "__main__":
        main()

