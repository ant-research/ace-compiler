@[TOC](TIMING.MD)

# API for timing in AVHC	
 - TIME_START ( )
 - TIME_TAKEN (testPoint)
 - TIME_TAKEN_NEST (testPoint, start)
 - TIME_LOG_FILE (fileName)

## Insert any test point to get the time spent

```
// Start of clock
TIME_START ( );

... ...

// Print time taken
TIME_TAKEN (testPoint);
```

## Gets the duration of a certain 2 points

```
// Start of clock
double start = TIME_START ( );

... ...

// Print time taken
TIME_TAKEN_NEST (testPoint, start);
```

## Prints the time consuming information to the log file

```
const char *fileName = "time_taken.log";

// Writes the terminal print to the log file
TIME_LOG_FILE(fileName);
```