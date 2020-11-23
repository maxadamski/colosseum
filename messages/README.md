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
Size  = XX XX XX XX
Tag   = XX 
Data* = XX XX XX XX
Type  = ...
Data  = ...

type    |hex |bin
--------|----|-----------
u8      | 00 | 0 000 0000
u16     | 01 | 0 000 0001
u32     | 02 | 0 000 0010
u64     | 03 | 0 000 0011
i8      | 04 | 0 000 0100
i16     | 05 | 0 000 0101
i32     | 06 | 0 000 0110
i64     | 07 | 0 000 0111
f32     | 08 | 0 000 1000
f64     | 09 | 0 000 1001
bool    | 0A | 0 000 1010
char    | 0B | 0 000 1011
---     | 0C | 0 000 1100
---     | 0D | 0 000 1101
---     | 0E | 0 000 1110
---     | 0F | 0 000 1111
arr T 1 | 1X | 0 001 TTTT [d1]                         
arr T 2 | 2X | 0 010 TTTT [d1] [d2]                    
arr T 3 | 3X | 0 011 TTTT [d1] [d2] [d3]               
arr T 4 | 4X | 0 100 TTTT [d1] [d2] [d3] [d4]          
arr T 5 | 5X | 0 101 TTTT [d1] [d2] [d3] [d4] [d5]     
arr T 6 | 6X | 0 110 TTTT [d1] [d2] [d3] [d4] [d5] [d6]
arr T N | 7X | 0 111 TTTT [Nd] [d1] [..] [dN]          
---     | 8X | 1 000 TTTT
tup T 1 | 9X | 1 001 TTTT [t1]                         
tup T 2 | AX | 1 010 TTTT [t1] [t2]                    
tup T 3 | BX | 1 011 TTTT [t1] [t2] [t3]               
tup T 4 | CX | 1 100 TTTT [t1] [t2] [t3] [t4]          
tup T 5 | DX | 1 101 TTTT [t1] [t2] [t3] [t4] [t5]     
tup T 6 | EX | 1 110 TTTT [t1] [t2] [t3] [t4] [t5] [t6]
tup T N | FX | 1 111 TTTT [Nt] [t1] [..] [tN]          
```

## Format syntax

```
T[t1, ..., tN]
()

T[]

T[%<=1024]

%3(u8 bool char)

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

