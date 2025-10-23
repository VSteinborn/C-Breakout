# C Breakout

'C Breakout', written in C.

<p align ="center" width="100%">
    <img src="./assets/demo.gif" />
</p>

![demo-video](./assets/demo.mp4)

## Dependencies

- [Raylib](https://www.raylib.com/)


## Build

Build the executable for your system via the following command:

```bash
gcc -O0 -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion -ggdb -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 ./src/util.c ./src/main.c
```

## Run the Program

Set the environment variables required to run raylib:

```bash
source env_var.bash
```

and then run the built executable:

```bash
./a.out # a.out gets built via the buid step
```
