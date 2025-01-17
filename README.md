**Snake Game (C Language)**

A simple implementation of the classic Snake game, designed for the Linux terminal.  
Learn more about the game concept: [Snake (video game genre)](https://en.wikipedia.org/wiki/Snake_(video_game_genre))  

**Gameplay:**  
- Eat food to grow the snake.
- Change direction using arrow keys.
- Avoid crashing into the snake's body.
- To pause press 'P'
- To quit press 'Q'

**Files:**  
- `snake` — Executable file.  
- `snake.c` — Source code.  

**How to Run:**  
1. Ensure the executable permission is set:  
   ```bash
   chmod +x snake
   ```  
2. Run the game in the terminal:  
   ```bash
   ./snake
   ```  

**How to Build:**  
Compile the source code using GCC:  
```bash
gcc snake.c -o snake
```  

**Game Modes & Command-Line Arguments:**  
- **God-mode**: Pass through the snake’s body without dying.  
  ```bash
  ./snake god-mode
  ```  
- **Lay-eggs**: The snake lays eggs from its tail.  
  ```bash
  ./snake lay-eggs
  ```  
- **Auto**: The snake moves automatically.  
  ```bash
  ./snake auto
  ```  
- **Matrix-mode**: Enter the Matrix—everything is styled like the Matrix, Neo...  
  ```bash
  ./snake matrix-mode
  ```  

--- 

Preview
![image](https://github.com/user-attachments/assets/dea7fdbc-527a-4447-b800-996743a1a47b)
