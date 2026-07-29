#!/usr/bin/env python3
import json
import os
import platform
import shutil
import subprocess
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DOWNLOADS = ROOT / "downloads"
INSTALL_DIR = ROOT / "Arduino IDE.app"
GITHUB_API = "https://api.github.com/repos/arduino/arduino-ide/releases/latest"


def run(cmd):
    print("+", " ".join(str(x) for x in cmd))
    subprocess.run(cmd, check=True)


def get_json(url):
    req = urllib.request.Request(url, headers={"User-Agent": "Sugeng-IOT-ArduinoIDE-Installer"})
    with urllib.request.urlopen(req) as res:
        return json.loads(res.read().decode("utf-8"))


def download(url, dest):
    req = urllib.request.Request(url, headers={"User-Agent": "Sugeng-IOT-ArduinoIDE-Installer"})
    with urllib.request.urlopen(req) as res, open(dest, "wb") as f:
        total = int(res.headers.get("Content-Length", "0"))
        done = 0
        while True:
            chunk = res.read(1024 * 1024)
            if not chunk:
                break
            f.write(chunk)
            done += len(chunk)
            if total:
                percent = done * 100 / total
                print(f"\rDownloading {percent:.1f}%", end="")
        print()


def select_asset(release):
    system = platform.system().lower()
    machine = platform.machine().lower()

    assets = release.get("assets", [])
    names = [a["name"] for a in assets]

    if system != "darwin":
        raise RuntimeError(f"Installer ini disiapkan untuk macOS. OS terdeteksi: {platform.system()}")

    # Arduino IDE macOS releases normally provide:
    # arduino-ide_x.x.x_macOS_64bit.dmg
    # arduino-ide_x.x.x_macOS_ARM64.dmg
    preferred = []
    if machine in ("arm64", "aarch64"):
        preferred = ["macOS_ARM64.dmg", "macOS_arm64.dmg", "macOS_AppleSilicon.dmg"]
    else:
        preferred = ["macOS_64bit.dmg", "macOS_x64.dmg", "macOS_Intel.dmg"]

    for suffix in preferred:
        for asset in assets:
            if asset["name"].endswith(suffix):
                return asset

    for asset in assets:
        name = asset["name"]
        if "macOS" in name and name.endswith(".dmg"):
            return asset

    raise RuntimeError("Asset Arduino IDE untuk macOS tidak ditemukan. Assets: " + ", ".join(names))


def mount_dmg(dmg_path):
    output = subprocess.check_output(
        ["hdiutil", "attach", "-nobrowse", "-readonly", str(dmg_path)],
        text=True,
    )
    mount_point = None
    for line in output.splitlines():
        if "/Volumes/" in line:
            mount_point = line[line.index("/Volumes/") :].strip()
    if not mount_point:
        raise RuntimeError("Mount point DMG tidak ditemukan.")
    return Path(mount_point)


def find_app(mount_point):
    apps = list(mount_point.glob("*.app"))
    if not apps:
        apps = list(mount_point.rglob("*.app"))
    if not apps:
        raise RuntimeError("File .app Arduino IDE tidak ditemukan di DMG.")
    return apps[0]


def copy_app(src_app, dest_app):
    if dest_app.exists():
        shutil.rmtree(dest_app)
    print(f"Copying {src_app} -> {dest_app}")
    shutil.copytree(src_app, dest_app, symlinks=True)


def install_board_esp32():
    arduino_cli = INSTALL_DIR / "Contents" / "Resources" / "app" / "lib" / "backend" / "resources" / "arduino-cli"
    if not arduino_cli.exists():
        print("arduino-cli bawaan Arduino IDE tidak ditemukan, skip install board ESP32.")
        return

    run([arduino_cli, "config", "init", "--overwrite"])
    run(
        [
            arduino_cli,
            "config",
            "set",
            "board_manager.additional_urls",
            "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json",
        ]
    )
    run([arduino_cli, "core", "update-index"])
    run([arduino_cli, "core", "install", "esp32:esp32"])
    run([arduino_cli, "lib", "install", "LiquidCrystal I2C"])


def main():
    DOWNLOADS.mkdir(parents=True, exist_ok=True)

    print("Arduino IDE adalah aplikasi GUI.")
    print("Installer ini akan download Arduino IDE dari GitHub release resmi Arduino.")
    print(f"Folder kerja: {ROOT}")

    release = get_json(GITHUB_API)
    asset = select_asset(release)

    dmg_path = DOWNLOADS / asset["name"]
    if not dmg_path.exists():
        print(f"Release: {release.get('tag_name')}")
        print(f"Asset: {asset['name']}")
        download(asset["browser_download_url"], dmg_path)
    else:
        print(f"Installer sudah ada: {dmg_path}")

    mount_point = None
    try:
        mount_point = mount_dmg(dmg_path)
        app = find_app(mount_point)
        copy_app(app, INSTALL_DIR)
    finally:
        if mount_point:
            run(["hdiutil", "detach", str(mount_point)])

    print(f"Arduino IDE berhasil disiapkan di: {INSTALL_DIR}")
    print("Mencoba install ESP32 board package dan library LCD I2C...")
    install_board_esp32()

    print()
    print("Selesai.")
    print(f"Buka Arduino IDE dengan command:")
    print(f'open "{INSTALL_DIR}"')


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr)
        sys.exit(1)