# Message Protocol

## Message structure

```
message = { size , tag , payload }
message = { size , tag , n , type_1 ... type_n , data_1 ... data_n }
type = prim | arr prim n d_1 ... d_n
prim = bool | i8 | i32 | u8 | u32 | f32 | f64
```

## Binary interface

```
Size = XX XX XX XX
Tag  = XX 
Argc = XX
Type = ...
Data = ...

type      | binary
----------|----------
u8        | 0000 0000
u32       | 0000 0001
i8        | 0000 0010
i32       | 0000 0011
f32       | 0000 0100
f64       | 0000 0101
bool      | 0000 0110
...       | ...
          | 1000 ----
arr T N   | 1NNN TTTT [ dim1 ] 
          | 1NNN TTTT [ dim1 ] [ dim2 ]
          | 1NNN TTTT ...
```

/*

type message format

	Aliases
	
	C   = u8
	U   = u32
	I   = i32
	F   = f32
	S   = v:u8:auto
	D:_ = v:u8:_

	When scanning vector or array dimensions, pointers to the dimensions must be passed in order **after** the pointer to the array!
*/


## C message API

```c
// typed send, returns bytes written
i32  send1(int f, i32 tag, char const *format, ...);

// byte send (without splitting arguments), returns bytes written
i32  send2(int f, i32 tag, void const *buffer, i32 size);

// typed recv waits for message of a given tag (tag >= 0)
void recv1(int f, i32  tag, char const *format, ...);

// byte recv waits for message of a given tag, returns bytes read
i32  recv2(int f, i32  tag, void *buffer, i32 size);

// byte recv waits for any message, returns bytes read, writes to buffer, and sets tag
i32  recv3(int f, i32 *tag, void *buffer, i32 size);
```


## Python message API pseudocode

```python
def msend(tag, arg_1, ..., arg_n):
	...

def mrecv(tag):
	if (tag', msg) in inbox
		return inbox.pop(msg)
	while (tag', msg) := recv()
		if tag == tag'
			return msg
		inbox.push((tag', msg))

```


## Pentago spec

```
next_moves() -> (X Y R: array i8 n)
next_states() -> (B: array i8 n 2*(2*board_size+1))
board_size() -> i8
do_move(x y r: i8)
commit_move(x y r: i8)
undo_move()

u32 n_states;
i8 states[36*36];
recv1(f, 1, "%a:i8:%u32:36", &states, %n_states)
```

