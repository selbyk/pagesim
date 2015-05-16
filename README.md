# Page Replacement Algorithms
Simulation of common page replacement algorithms used by
operating systems to manage memory usage.

This is built upon a programming assignment given in Operating Systems (CSC 4301) at LSU, instructed by Feng Chen.

Algorithms Implemented
- Random
- FIFO
- LRU
- CLOCK
- NFU
- NFU with aging

Todo
- Improve configuration ability
- Add better paging models than just grabbing random refs/self-defined page ref strings
- Optimal algorithm
- Second-chance algorithm
- Most recently used
- Most frequently used
- Compare all other algorithms to Optimal algorithm

I used a linux system library for fun, if someone wants to get
it running on Macs that'd be awesome.

Pull requests always welcome. =)

## Building

```bash
make
```

## Running

```bash
./pagesim <algorithm: {ALL, LRU, CLOCK}> <# page frames: integer greater than 0> <debug: 0 or 1, default 0>
```

## Example Usage

```bash
./pagesim ALL 10
```

## Example Output

```bash
[selby@dux os-2]$ ./pagesim A 10
RANDOM Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

FIFO Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

LRU Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

CLOCK Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

NFU Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

AGING Algorithm
Frames in Mem: 10, Hits: 0, Misses: 1, Hit Ratio: 0.000000
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        10        _        _        _        _        _        _        _        _        _
Extra    :         0        0        0        0        0        0        0        0        0        0
Time     :  31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061 31806061

...

RANDOM Algorithm
Frames in Mem: 10, Hits: 827, Misses: 173, Hit Ratio: 0.827828
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :         5        0        7        6        3        2        4       10        9       11
Extra    :       986      995      982      992      985      999      988      991      998      996
Time     :  31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962

FIFO Algorithm
Frames in Mem: 10, Hits: 829, Misses: 171, Hit Ratio: 0.829830
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :         4        9        6        3        2       11        7        0        1       10
Extra    :       988      998      992      985      999      996      982      995      997      991
Time     :  31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962

LRU Algorithm
Frames in Mem: 10, Hits: 815, Misses: 185, Hit Ratio: 0.815816
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :        11        9        5        3        6        2        7        4        1       10
Extra    :       996      998      986      985      992      999      982      988      997      991
Time     :  31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962

CLOCK Algorithm
Frames in Mem: 10, Hits: 813, Misses: 187, Hit Ratio: 0.813814
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :         9        1        5       11        0        4        2        3        6       10
Extra    :         0        0        1        1        1        1        0        1        1        1
Time     :  31805961 31805961 31805961 31805961 31805961 31805961 31805961 31805961 31805961 31805961

NFU Algorithm
Frames in Mem: 10, Hits: 809, Misses: 191, Hit Ratio: 0.809810
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :         8        9        6        3        2       11        5        0        1       10
Extra    :        72       68       75       91       84       81       96       80       92        1
Time     :  31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962

AGING Algorithm
Frames in Mem: 10, Hits: 811, Misses: 189, Hit Ratio: 0.811812
Frame #  :         0        1        2        3        4        5        6        7        8        9
Page Ref :         1       10        0        5        9        4        7        3        2       11
Extra    :   2812504    70806   625000     1221        0        0       76      610 10000028  1250009
Time     :  31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962 31805962

RANDOM Algorithm
Frames in Mem: 10, Hits: 827, Misses: 173, Hit Ratio: 0.827000
FIFO Algorithm
Frames in Mem: 10, Hits: 829, Misses: 171, Hit Ratio: 0.829000
LRU Algorithm
Frames in Mem: 10, Hits: 815, Misses: 185, Hit Ratio: 0.815000
CLOCK Algorithm
Frames in Mem: 10, Hits: 813, Misses: 187, Hit Ratio: 0.813000
NFU Algorithm
Frames in Mem: 10, Hits: 809, Misses: 191, Hit Ratio: 0.809000
AGING Algorithm
Frames in Mem: 10, Hits: 811, Misses: 189, Hit Ratio: 0.811000

```
