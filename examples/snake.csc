# Snake (贪吃蛇) — vivaldi 2D Engine
# WASD/Arrows:Move  4/5:Speed  P:Auto  R:Reset

import vivaldi
using vivaldi

# ========== Constants ==========
constant GW = 20
constant GH = 20
constant CS = 24               # Cell pixel size
constant BOARD_X = 16
constant BOARD_Y = 16
constant BOARD_W = GW * CS
constant BOARD_H = GH * CS
constant PANEL_X = BOARD_X + BOARD_W + 20

# ========== Game State ==========
var snake = new array
var food = {0, 0}
var dir = {1, 0}
var next_dir = {1, 0}
var score = 0
var level = 1
var base_intv = 0.10
var move_timer = 0.0
var game_over = false
var auto_play = false

function init_game()
    snake.clear()
    var sx = to_integer(GW / 2) ; var sy = to_integer(GH / 2)
    snake.push_back({sx, sy})
    for i = 1, i < 3, ++i ; snake.push_back({sx - i, sy}) ; end
    dir = {1, 0} ; next_dir = {1, 0}
    score = 0 ; level = 1 ; move_timer = 0.0 ; game_over = false ; auto_play = false
    gen_food()
end

function gen_food()
    var occ = new hash_map
    for i = 0, i < snake.size, ++i ; occ.insert(to_string(snake[i][0]) + "," + to_string(snake[i][1]), true) ; end
    if snake.size >= GW * GH ; game_over = true ; return ; end
    var fx = 0 ; var fy = 0 ; var found = false ; var tries = 0
    while not found and tries < GW * GH * 10
        fx = random_int(0, GW - 1) ; fy = random_int(0, GH - 1)
        if not occ.exist(to_string(fx) + "," + to_string(fy)) ; found = true ; end
        tries = tries + 1
    end
    if not found
        for fy = 0, fy < GH, ++fy
            for fx = 0, fx < GW, ++fx
                if not occ.exist(to_string(fx) + "," + to_string(fy)) ; found = true ; break ; end
            end
            if found ; break ; end
        end
    end
    food[0] = fx ; food[1] = fy
end

function check_collision(x, y)
    if x < 0 or x >= GW or y < 0 or y >= GH ; return true ; end
    for i = 0, i < snake.size - 1, ++i
        if snake[i][0] == x and snake[i][1] == y ; return true ; end
    end
    return false
end

function move_snake()
    var h = snake[0] ; var nx = h[0] + dir[0] ; var ny = h[1] + dir[1]
    if check_collision(nx, ny) ; game_over = true ; return ; end
    snake.push_front({nx, ny})
    if nx == food[0] and ny == food[1]
        score = score + 10
        if score % 50 == 0 ; level = level + 1 ; end
        gen_food()
    else
        snake.pop_back()
    end
end

# BFS pathfinding for AI
function bfs_path(sx, sy, tx, ty)
    var blocked = new hash_map
    for i = 0, i < snake.size, ++i ; blocked.insert(to_string(snake[i][0]) + "," + to_string(snake[i][1]), true) ; end
    var visited = new hash_map
    var queue = new array ; queue.push_back({sx, sy, new array})
    var qh = 0 ; var dirs = {{0,-1},{0,1},{-1,0},{1,0}}
    while qh < queue.size
        var cur = queue[qh] ; qh = qh + 1
        var ck = to_string(cur[0]) + "," + to_string(cur[1])
        if not visited.exist(ck)
            visited.insert(ck, true)
            if cur[0] == tx and cur[1] == ty ; return cur[2] ; end
            for d = 0, d < 4, ++d
                var nx = cur[0] + dirs[d][0] ; var ny = cur[1] + dirs[d][1]
                if nx >= 0 and nx < GW and ny >= 0 and ny < GH
                    var nk = to_string(nx) + "," + to_string(ny)
                    if not blocked.exist(nk) and not visited.exist(nk)
                        var np = new array
                        for k = 0, k < cur[2].size, ++k ; np.push_back(cur[2][k]) ; end
                        np.push_back({nx, ny}) ; queue.push_back({nx, ny, np})
                    end
                end
            end
        end
    end
    return new array
end

function ai_decide()
    var h = snake[0] ; var path = bfs_path(h[0], h[1], food[0], food[1])
    if path.size > 0 ; var nxt = path[0] ; return {nxt[0] - h[0], nxt[1] - h[1]} ; end
    # No path to food — chase tail via longest safe detour
    var tail = snake[snake.size - 1] ; var dl = {{0,-1},{0,1},{-1,0},{1,0}}
    var best_dir = dir ; var best_len = -1
    for i = 0, i < 4, ++i
        var nx = h[0] + dl[i][0] ; var ny = h[1] + dl[i][1]
        if not check_collision(nx, ny)
            var tp = bfs_path(nx, ny, tail[0], tail[1])
            if tp.size > best_len ; best_len = tp.size ; best_dir = dl[i] ; end
        end
    end
    if best_len >= 0 ; return best_dir ; end
    # Desperate: any safe move
    for i = 0, i < 4, ++i
        if not check_collision(h[0] + dl[i][0], h[1] + dl[i][1]) ; return dl[i] ; end
    end
    return dir
end

# ========== Drawing ==========

constant SNAKE_R = 10.0          # snake body radius
constant SNAKE_HALF = CS / 2.0   # cell center offset

function cell_cx(gx) ; return BOARD_X + gx * CS + SNAKE_HALF ; end
function cell_cy(gy) ; return BOARD_Y + gy * CS + SNAKE_HALF ; end

function draw_board()
    fill_rect(BOARD_X - 4, BOARD_Y - 4, BOARD_W + 8, BOARD_H + 8, C_SURFACE, 8)
    draw_rect(BOARD_X - 4, BOARD_Y - 4, BOARD_W + 8, BOARD_H + 8, C_BORDER, 1.5, 8)
    for i = 1, i < GW, ++i
        var x = BOARD_X + i * CS
        draw_line(x, BOARD_Y, x, BOARD_Y + BOARD_H, alpha(C_BORDER, 0.2), 0.5)
    end
    for j = 1, j < GH, ++j
        var y = BOARD_Y + j * CS
        draw_line(BOARD_X, y, BOARD_X + BOARD_W, y, alpha(C_BORDER, 0.2), 0.5)
    end
end

function draw_snake()
    var head_c = {0.3, 0.95, 0.45, 1.0}
    var body_c = C_GREEN
    var ln = snake.size
    # Body: thick lines connecting consecutive segments
    for i = 0, i < ln - 1, ++i
        var x1 = cell_cx(snake[i][0]) ; var y1 = cell_cy(snake[i][1])
        var x2 = cell_cx(snake[i+1][0]) ; var y2 = cell_cy(snake[i+1][1])
        draw_line(x1, y1, x2, y2, body_c, SNAKE_R * 2)
    end
    # Joints: circles over each segment for smooth look
    for i = 1, i < ln, ++i
        var x = cell_cx(snake[i][0]) ; var y = cell_cy(snake[i][1])
        fill_circle(x, y, SNAKE_R, body_c)
    end
    # Head
    var hx = cell_cx(snake[0][0]) ; var hy = cell_cy(snake[0][1])
    fill_circle(hx, hy, SNAKE_R + 1, head_c)
    # Eyes
    var er = 3.0 ; var pr = 1.5 ; var eoff = SNAKE_R * 0.55
    var ex1 = hx ; var ey1 = hy ; var ex2 = hx ; var ey2 = hy
    if dir[0] == 1  ; ex1 = hx + eoff ; ey1 = hy - eoff ; ex2 = hx + eoff ; ey2 = hy + eoff ; end
    if dir[0] == -1 ; ex1 = hx - eoff ; ey1 = hy - eoff ; ex2 = hx - eoff ; ey2 = hy + eoff ; end
    if dir[1] == -1 ; ex1 = hx - eoff ; ey1 = hy - eoff ; ex2 = hx + eoff ; ey2 = hy - eoff ; end
    if dir[1] == 1  ; ex1 = hx - eoff ; ey1 = hy + eoff ; ex2 = hx + eoff ; ey2 = hy + eoff ; end
    fill_circle(ex1, ey1, er, C_WHITE)
    fill_circle(ex2, ey2, er, C_WHITE)
    fill_circle(ex1, ey1, pr, C_BLACK)
    fill_circle(ex2, ey2, pr, C_BLACK)
end

function draw_food()
    var cx = cell_cx(food[0]) ; var cy = cell_cy(food[1])
    draw_circle(cx, cy, 13, alpha(C_RED, 0.2), 2)
    fill_circle(cx, cy, 9, C_RED)
    fill_circle(cx - 2, cy - 2, 3, alpha(C_WHITE, 0.35))
end

function draw_panel()
    var px = PANEL_X ; var py = BOARD_Y
    fill_rect(px, py, 120, 250, C_SURFACE, 6)
    draw_rect(px, py, 120, 250, C_BORDER, 1, 6)
    var ly = py + 14
    draw_text(px + 12, ly, "SCORE", C_GRAY, 13) ; ly = ly + text_height(13) + 4
    draw_text(px + 12, ly, to_string(score), C_WHITE, 24) ; ly = ly + text_height(24) + 6
    draw_text(px + 12, ly, "LEVEL", C_GRAY, 13) ; ly = ly + text_height(13) + 4
    draw_text(px + 12, ly, to_string(level), C_YELLOW, 24) ; ly = ly + text_height(24) + 6
    draw_text(px + 12, ly, "SPEED", C_GRAY, 13) ; ly = ly + text_height(13) + 4
    var spd = to_integer(base_intv * 1000)
    draw_text(px + 12, ly, to_string(spd) + "ms", C_CYAN, 20) ; ly = ly + text_height(20) + 6
    var ac = C_GRAY ; if auto_play ; ac = C_GREEN ; end
    draw_text(px + 12, ly, "AUTO", C_GRAY, 13) ; ly = ly + text_height(13) + 4
    draw_text(px + 12, ly, "ON", ac, 22) ; ly = ly + text_height(22) + 8
    draw_text(px + 12, ly + 2, "WASD/Arrows", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px + 12, ly + 2, "P      Auto", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px + 12, ly + 2, "4/5    Speed", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px + 12, ly + 2, "R      Reset", C_GRAY, 11)
end

function draw_bottom_bar()
    var by = BOARD_Y + BOARD_H + 10 ; var bar_h = to_integer(text_height(13) + 14)
    fill_rect(BOARD_X - 4, by, BOARD_W + 8, bar_h, C_SURFACE, 4)
    var txt = "WASD/Arrows:Move  4/5:Speed  P:Auto  R:Reset"
    draw_text(BOARD_X + to_integer((BOARD_W - text_width(txt, 13)) / 2), by + to_integer((bar_h - text_height(13)) / 2), txt, C_GRAY, 13)
end

function draw_game_over()
    fill_rect(BOARD_X - 4, BOARD_Y - 4, BOARD_W + 8, BOARD_H + 8, alpha(C_BLACK, 0.7), 8)
    var cx = BOARD_X + BOARD_W / 2.0 ; var cy = BOARD_Y + BOARD_H / 2.0
    var t1 = "GAME OVER" ; var t2 = "Score: " + to_string(score) ; var t3 = "Press R to Restart"
    var total_h = text_height(28) + text_height(18) + text_height(13) + 16
    var y = cy - total_h / 2
    draw_text(cx - text_width(t1, 28) / 2, y, t1, C_RED, 28) ; y = y + text_height(28) + 10
    draw_text(cx - text_width(t2, 18) / 2, y, t2, C_WHITE, 18) ; y = y + text_height(18) + 6
    draw_text(cx - text_width(t3, 13) / 2, y, t3, C_GRAY, 13)
end

# ========== Main Loop ==========
init("Snake", 660, 540)
init_game()

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.05 ; dt = 0.05 ; end

    if key_pressed(K_R) ; init_game() ; end
    if key_pressed(K_P) ; auto_play = not auto_play ; end
    if not game_over
        if key_pressed(K_W) or key_pressed(K_UP) ; if dir[1] != 1 ; next_dir = {0, -1} ; end ; end
        if key_pressed(K_S) or key_pressed(K_DOWN) ; if dir[1] != -1 ; next_dir = {0, 1} ; end ; end
        if key_pressed(K_A) or key_pressed(K_LEFT) ; if dir[0] != 1 ; next_dir = {-1, 0} ; end ; end
        if key_pressed(K_D) or key_pressed(K_RIGHT) ; if dir[0] != -1 ; next_dir = {1, 0} ; end ; end
        if key_pressed(K_4) and base_intv < 2.0 ; base_intv = base_intv + 0.01 ; end
        if key_pressed(K_5) and base_intv > 0.03 ; base_intv = base_intv - 0.01 ; end
        if auto_play
            var ad = ai_decide()
            if ad[0] != -dir[0] or ad[1] != -dir[1] ; next_dir = ad ; end
        end
        move_timer = move_timer + dt
        var intv = base_intv
        for lv = 1, lv < level, ++lv ; intv = intv * 0.95 ; end
        while move_timer >= intv
            move_timer = move_timer - intv
            dir = next_dir
            move_snake()
            if game_over ; break ; end
        end
    end

    draw_board()
    draw_snake()
    draw_food()
    draw_panel()
    draw_bottom_bar()
    if game_over ; draw_game_over() ; end
    end()
end
