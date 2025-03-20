# 🦅 KIIT-Finder 
Enter roll number and get the corresponding details.

## Compilation Steps:
Make sure to install `libcurl` to properly run the executable.\

```bash
sudo apt install libcurl4-openssl-dev gcc
git clone https://github.com/prajwal-panth/kiit-finder
cd kiit-finder
gcc kiit-finder.c -lcurl -o kiit.bin
./kiit.bin
```
Put required roll numbers in `input.txt` each in new-line and the corresponding details will be saved in `output.txt` in CSV format.