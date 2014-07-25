import os

if __name__=='__main__':
    for subdir, dirs, files in os.walk('./'):
        for file in files:
            os.rename(file, file.lower())
