# qapl

qapl is a Qt-based interface to GNU APL.  In effect, it replaces the terminal
emulator in which APL is usually run but offers the following additional
capabilities:

+ Access to external editors such as emacs or gvim.
+ GUI-style access to APL )load, )save, )copy, etc., operations.
+ &gt;&gt; function to export expression results to a file or >>> to append to a file.
+ |> function to call an external function with expression results

qapl offers two major capabilities, a terminal emulator for user interaction,
and a means of using external editors like emacs and gvim to edit APL
functions and lambda.

As a terminal emulator, qapl interfaces with the APL engine through libapl,
avoiding potential interactions with the standard APL CLI.  For the editor
interface, qapl uses Qt processes rather than APL native functions and
operating system processes.

<h1>Output redirection and pipes</h1>

qapl provides what look like two additional APL functions, though they're
implemented in qapl rather than in the APL engine.

<h2>File redirection:  >> and >>></h2>

The first of these functions, in dyadic operation, provides redirection of its
left expression to an external file.  For example, 

<p style="text-align: center;">⍳8>>fubar</p>

will create file fubar containing:

<p style="text-align: center;">0 1 2 3 4 5 6 7</p>

Files maybe appended with the >>> operator:

<p style="text-align: center;">>⍳8>>fubar</p>

results in:
<p style="text-align: center;">0 1 2 3 4 5 6 7<br>0 1 2 3 4 5 6 7</p>

The redirection function can also be used monadically:

<p style="text-align: center;">>>fubar</p>

Which will leave the file in existence but will make it zero length.

<h2>Piping:  |></h2>

The piping function pipes the results of the left expression to the given
external command and any standard output of that command will be captured.
For example:

```
	⍳8|>wc
1       8      16
```

Arguments may be provided as well:

```
	wc -w -l
1       8
```


The output can be assigned to an APL variable:

```
	⍳8|>z←wc -w -l
      z
1 8

      ⍴z
2
```

If a command doesn't accept or doesn't need standard input, the left argument
may be omitted:

```
      |>date
Fri Oct  8 02:20:49 PM EDT 2021
```

If the standard output of a command is a string, as in this case, the output
may be assigned to a variable by enclosing the command in quotes:

```
      |>z←'date'
      z
Fri Oct  8 02:23:34 PM EDT 2021
```

(This also works with numeric output:

```
      ⍳8|>z←wc -w -l
      z
1 8

      ⍴z
2

      ⍳8|>z←'wc -w -l'
      z
      1       8

      ⍴z
15
)

Argument substitutions maybe made to piped commands through the use of
backquotes:

```
      opta←'-w'
      optb←'-l'
      ⍳8|>wc
      1       8      16

      ⍳8|>wc `opta` `optb`
      1       8

      optb←'-c'
      ⍳8|>wc `opta` `optb`
      8      16
```

or

```
      arga←'this is a string'
      |>echo `arga`
"this is a string"

      arga←'this is a different string'
      |>echo `arga`
"this is a different string"
```

or

```
      |>echo `'iota 8 equals '`  `⍳8`
 iota 8 equals   "0 1 2 3 4 5 6 7"
```
