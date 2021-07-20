# ZsetStorage

A storage engine that supports set structure.

## File Tree
```.
├── bin
│   ├── main
│   └── stress
├── data
│   └── dump_file
├── main.cpp
├── README.md
├── run.sh
├── skiplist.h
└── stress-test
    ├── stress_test.cpp
    └── stress_test.sh
```

## Stress Test
Environment：Ubuntu18.04, cpu: 1, memory: 2G

### insert & search

insert random key

| tree high | element num (w) | insert timecost(s) | search timeoutcost(s) |
|---|---|---|---|
|15|10 |0.0902468 |0.0880118 |
|15|50 |1.14008 |0.898742 |
|15|100 |3.16509 |2.28648 |

## Run
```bash
./bin/main  // run 

./bin/stress    // stress test
```