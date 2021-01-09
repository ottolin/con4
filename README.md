# Small connected 4 game in C++

## Major design decisions

Use padding to stuff game board to avoid boundary checking.
Assuming the board size can be changed easily, i.e. avoid loops related to size of game board:
* Store next row index for each filling column in game state
* Check for connections related to last filled position instead of loopping the whole board

### Building the builder docker only
`docker build --target build -t ottolin/con4builder .`

### Building the runtime
`docker build -t ottolin/con4 .`
