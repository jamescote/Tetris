# Tetris
Main Repository for Tetris project; Atari 2600.

2018 Notes: From what I remember, I got custom Atari interrupts working for this project. However, there is a race condition bug in one of those interrupt functions that I never fixed. It would rarely, if ever, occur. But there is a possibility, so it should be fixed at some point. I loved working on the Music for this assignment. Figuring out the timing updates for the iconic Tetris theme was a lot of fun. will need an atari emulator to run.

Also, There is a small bug with rendering sometimes. When a Tetromino clicks in, it will sometimes flicker as it is drawn on one of the buffers for double buffering, but not both. Minor bug.

Tetriminos are stored as an array of bytes and are moved using bitshifting and bit logic algorithms. That information is used to determine what to draw on screen. I enjoy problems like this at this base level.
