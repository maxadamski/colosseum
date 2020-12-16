import os
from pathlib import Path
import zipfile
import shutil
import subprocess

FILES_DIR = os.path.join(os.getcwd(), 'files')
GAMES_DIR = os.path.join(FILES_DIR, 'games')
SUBMISSIONS_DIR = os.path.join(FILES_DIR, 'submissions')


def unpack_temp_zip_file(file_path, destination_path):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(destination_path)
    os.unlink(file_path)


def clear_dir_contents(directory):
    for root, dirs, files in os.walk(directory):
        for f in files:
            os.unlink(os.path.join(root, f))
        for d in dirs:
            shutil.rmtree(os.path.join(root, d))


def get_game_directories(game_id, init=False):
    game_dir = os.path.join(GAMES_DIR, str(game_id))
    game_files_dir = os.path.join(game_dir, 'judge')
    if init:
        Path(game_files_dir).mkdir(parents=True, exist_ok=True)
    return game_dir, game_files_dir


def get_game_submission_directory(game_id, submission_id, init=False):
    game_submission_dir = os.path.join(GAMES_DIR, str(game_id), 'submissions', str(submission_id))
    if init:
        Path(game_submission_dir).mkdir(parents=True, exist_ok=True)
    return game_submission_dir


def get_submission_directory(submission_id, init=False):
    submission_dir = os.path.join(SUBMISSIONS_DIR, str(submission_id))
    if init:
        Path(submission_dir).mkdir(parents=True, exist_ok=True)
    return submission_dir


def save_and_unzip_files(executable_dir, file, name=None):
    ext = os.path.splitext(file.filename)[1]
    file_path = os.path.join(executable_dir, str(name) + ext)
    with open(file_path, 'wb+') as f:
        f.write(file.file.read())
        f.close()
    if ext == '.zip':
        unpack_temp_zip_file(file_path, executable_dir)


def compile_files(dir_path, file, env_id):
    if env_id == 0:
        cmd = "gcc {}".format(file)
    elif env_id == 1:
        cmd = "c++ {}".format(file)
    elif env_id == 2:
        return 0
    else:
        return -1
    return subprocess.call(cmd, shell=True, cwd=dir_path)
