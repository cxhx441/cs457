import os

def rm_tex(top_dir_path, file_name):
    for root, dirs, files in os.walk(top_dir_path):
        for file in files:
            if file == file_name:
                file_path = os.path.join(root, file_name)
                os.remove(file_path)
                print(f"Removed file: {file_path}")

dir_path = "./"
file_name = "noise2d.064.tex"
rm_tex(dir_path, file_name)
print()
file_name = "noise3d.064.tex"
rm_tex(dir_path, file_name)
file_name = "errorlog.txt"
rm_tex(dir_path, file_name)

