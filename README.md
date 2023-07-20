A simple C program that can help empirically determine your CPU's cache size. Inspired by [Chapter 7.4 of ThinkOS "measuring cache performance"](https://greenteapress.com/thinkos/html/thinkos009.html#sec44).

Plotting is done in Python using [Plotly](https://plotly.com):
```shell
pip install plotly
```

The main program `cache.c` iterates an array at various sizes and strides and outputs the elapsed times for each stride/size pair as a csv, which can be directly piped into `plot.py` to plot the result:
```shell
gcc cache.c && ./a.out | python plot.py
```
This should output a file `plot.html`, which you can open in your browser.
