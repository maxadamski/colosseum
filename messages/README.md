# Message Protocol

## Message structure

```
message = { size , tag , payload }
message = { size , tag , n , offset_1 ... offset_n , toffset_1 ... toffset_n , type_1 ... type_n , data_1 ... data_n }
type = { str, n } | { arr, prim, n, d_1 ... d_n }
prim = bool | i8 | i16 | i32 | i64 | u8 | u16 | u32 | u64
n d_1 ... d_n : u32
```


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
```

