# Webe

## Intorduction

`Webe` is a web server. I wrote it to hand it in as homework...

## How to Build

```bash
$ git clone https://github.com/Fugoes/Webe.git
$ cd Webe
$ mkdir cmake
$ cd cmake
$ cmake ..
$ make
```

To get release version which run much faster, use `cmake -DCMAKE_BUILD_TYPE=Release ..` instead of `cmake ..`.

## How to Use

```bash
$ cd Webe/cmake
$ ./webectl
Usage: webectl load-module <module>
       webectl unload-module <module>
       webectl list-server
       webectl stop-server
       webectl start-server <processes> [ OPTIONS ]
where  OPTIONS = { -a <address> | -p <port> }
```

You could directly run `./webed` instead. By default, it will load 404 module and info module.

## Demo

```shell
$ cd Webe/cmake
$ ln -s ../example/static static
$ ./webectl start-server 4 -a 0.0.0.0 -p 8080
$ ./webectl list-server
        PID
 ----------
       1906
       1907
       1905
       1908
$ ./webectl load-module file_server
```

Now browse `http://127.0.0.1:8080/info`.

To stop all server:

```shell
$ ./webectl stop-server
        PID  STATUS
 ---------- -------
       1906 STOPPED
       1907 STOPPED
       1905 STOPPED
       1908 STOPPED
```