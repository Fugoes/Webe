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
You could also do some benchmark with ab from Apache2:

```shell
$ ab -k -n 1000000 -c 100 http://127.0.0.1:8080/static/nice.css
...
Concurrency Level:      100
Time taken for tests:   7.025 seconds
Complete requests:      1000000
Failed requests:        0
Non-2xx responses:      1000000
Keep-Alive requests:    1000000
Total transferred:      302000000 bytes
HTML transferred:       151000000 bytes
Requests per second:    142340.22 [#/sec] (mean)
Time per request:       0.703 [ms] (mean)
Time per request:       0.007 [ms] (mean, across all concurrent requests)
Transfer rate:          41979.25 [Kbytes/sec] received
...
```

This result is tested with an i5 4210H CPU.