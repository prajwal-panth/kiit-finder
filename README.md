# 🦅 KIIT-Finder
Find KIIT student details solely from roll numbers.
Original [kiit-finder](https://github.com/prajwal-panth/kiit-finder), written in C with `lib-curl`. 

## Compilation Steps:
Make sure to install `libcurl` to properly run the executable.
```bash
sudo apt install libcurl4-openssl-dev gcc
git clone https://github.com/prajwal-panth/kiit-finder
cd kiit-finder
gcc kiit.c -lcurl -o kiit.bin
./kiit.bin
```
Put required roll numbers in `input.txt` each in new-line and the corresponding details will be saved in `output.txt` in CSV format.

**Note**: KIIT is a University of India. [Visit-KIIT](https://kiit.ac.in/)
