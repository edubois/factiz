# Product of large prime factorizer

This is something I invented when I was a student in 2008. At that time, there was a bug I accidentally introduced and which I did not find, thanks to Mistral AI I was able to make it working again.

Disclaimer: I have no idea how relevant this is, this is not my field of study, but at the time I invented this, I really enjoyed the expedition.

## Compilation
* Requirement: boost somewhere
* Advise: download boost as zip and compile by hand, alternative is to run: sudo apt install libboost-multiprecision-dev


```
mkdir build
cd build
cmake ..       -DBoost_INCLUDE_DIR=/path/to/compiled/boost_1_75_0/ -DBoost_NO_SYSTEM_PATHS=ON
make -j9
```

## Running
```
./factizExample1 100000980001501
P*Q = 10000079 * 10000019
Time taken: 0 ms
```
