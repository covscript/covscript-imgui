# Tetris (俄罗斯方块) — vivaldi 2D Engine
# W/Up:Spin  A/D/LR:Move  S/Down:Drop  Space:HardDrop  C:Hold  4/5:Speed  P:Auto  R:Reset

import vivaldi
using vivaldi

# ========== Layout ==========
constant BW = 10
constant BH = 20
constant CS = 26               # cell pixel size
constant BOARD_X = 110
constant BOARD_Y = 30
constant BOARD_W = BW * CS
constant BOARD_H = BH * CS
constant CELL_PAD = 1          # padding inside cell for 3D effect blocks
constant PANEL_X = 392
constant HOLD_X = 16
constant HOLD_Y = BOARD_Y

# ========== Block Definitions ==========
namespace blocks
    var I = {{1,1,1,1}}
    var O = {{2,2},{2,2}}
    var T = {{3,3,3},{0,3,0}}
    var S = {{0,4,4},{4,4,0}}
    var Z = {{5,5,0},{0,5,5}}
    var J = {{6,6,6},{0,0,6}}
    var L = {{7,7,7},{7,0,0}}
end

var block_bag = {blocks.I, blocks.O, blocks.T, blocks.S, blocks.Z, blocks.J, blocks.L}

# ========== Piece Colors (fill, light edge, dark edge) ==========
function piece_colors(id)
    if id == 1 ; return {{0.1, 0.85, 0.9, 1.0}, {0.4, 1.0, 1.0, 1.0}, {0.0, 0.6, 0.7, 1.0}} ; end  # I cyan
    if id == 2 ; return {{0.9, 0.8, 0.1, 1.0},  {1.0, 0.95, 0.3, 1.0}, {0.7, 0.6, 0.0, 1.0}} ; end  # O yellow
    if id == 3 ; return {{0.65, 0.2, 0.8, 1.0}, {0.8, 0.4, 1.0, 1.0},  {0.45, 0.1, 0.6, 1.0}} ; end # T purple
    if id == 4 ; return {{0.15, 0.8, 0.3, 1.0}, {0.3, 1.0, 0.5, 1.0},  {0.05, 0.6, 0.15, 1.0}} ; end# S green
    if id == 5 ; return {{0.8, 0.2, 0.2, 1.0},  {1.0, 0.35, 0.4, 1.0}, {0.6, 0.05, 0.1, 1.0}} ; end # Z red
    if id == 6 ; return {{0.2, 0.4, 0.9, 1.0},  {0.4, 0.6, 1.0, 1.0},  {0.1, 0.25, 0.7, 1.0}} ; end # J blue
    if id == 7 ; return {{0.9, 0.45, 0.1, 1.0}, {1.0, 0.65, 0.3, 1.0}, {0.7, 0.3, 0.05, 1.0}} ; end # L orange
    return {{0.5,0.5,0.5,1.0},{0.7,0.7,0.7,1.0},{0.3,0.3,0.3,1.0}}
end

# ========== Game State ==========
var board = null
var now = null ; var now_id = 0 ; var now_x = 0 ; var now_y = 0
var next_id = 0 ; var store_id = 0
var has_store = false ; var store_used = false
var score = 0 ; var level = 1 ; var lines = 0
var base_drop = 1.0 ; var drop_intv = 1.0 ; var drop_timer = 0.0
var game_over = false ; var auto_play = false
var ai_target = null ; var ai_rot_done = false

# ========== Game Logic ==========

function create_board()
    board = new array
    for i = 0, i < BH, ++i ; board[i] = new array ; for j = 0, j < BW, ++j ; board[i][j] = 0 ; end ; end
end

function new_piece()
    now_id = next_id ; now = clone(block_bag[now_id])
    now_x = to_integer((BW - now[0].size) / 2) ; now_y = 0
    next_id = random_int(0, 6) ; store_used = false
    if not moveable(now_x, now_y, now) ; game_over = true ; end
end

function init_game()
    create_board() ; next_id = random_int(0, 6)
    store_id = 0 ; has_store = false ; store_used = false
    score = 0 ; level = 1 ; lines = 0
    drop_intv = base_drop ; drop_timer = 0.0
    game_over = false ; auto_play = false
    ai_target = null ; ai_rot_done = false
    new_piece()
end

function moveable(x, y, b)
    if y + b.size > BH or x < 0 or x + b[0].size > BW ; return false ; end
    for i = 0, i < b.size, ++i
        for j = 0, j < b[0].size, ++j
            if b[i][j] != 0
                var by = i + y ; var bx = j + x
                if by >= 0 ; if board[by][bx] != 0 ; return false ; end ; end
            end
        end
    end
    return true
end

function spin_piece()
    var ls = new array ; var rows = now[0].size ; var cols = now.size
    for i = 0, i < rows, ++i ; ls[i] = new array ; for j = 0, j < cols, ++j ; ls[i][j] = now[cols - j - 1][i] ; end ; end
    if moveable(now_x, now_y, ls) ; now = ls ; return ; end
    var kicks = {-1, 1, -2, 2}
    for k = 0, k < kicks.size, ++k
        if moveable(now_x + kicks[k], now_y, ls) ; now = ls ; now_x = now_x + kicks[k] ; return ; end
    end
end

function lock_piece()
    for i = 0, i < now.size, ++i
        for j = 0, j < now[0].size, ++j
            if now[i][j] != 0
                var by = i + now_y ; var bx = j + now_x
                if by >= 0 and by < BH ; board[by][bx] = now[i][j] ; end
            end
        end
    end
    clear_lines() ; new_piece()
end

function clear_lines()
    var cleared = 0 ; var i = BH - 1
    while i >= 0
        var full = true
        for j = 0, j < BW, ++j ; if board[i][j] == 0 ; full = false ; break ; end ; end
        if full
            cleared = cleared + 1
            for r = i, r > 0, --r ; for c = 0, c < BW, ++c ; board[r][c] = board[r - 1][c] ; end ; end
            for c = 0, c < BW, ++c ; board[0][c] = 0 ; end
        else ; i = i - 1 ; end
    end
    if cleared > 0
        var pts = {0, 100, 300, 500, 800}
        if cleared > 4 ; cleared = 4 ; end
        score = score + pts[cleared] * level ; lines = lines + cleared
        level = to_integer(lines / 10) + 1
        drop_intv = base_drop
        for lv = 1, lv < level, ++lv ; drop_intv = drop_intv * 0.85 ; end
    end
end

function move_left()  ; if moveable(now_x - 1, now_y, now) ; now_x = now_x - 1 ; end ; end
function move_right() ; if moveable(now_x + 1, now_y, now) ; now_x = now_x + 1 ; end ; end
function soft_drop()  ; if moveable(now_x, now_y + 1, now) ; now_y = now_y + 1 ; return true ; end ; return false ; end

function hard_drop()
    var landed = now_y
    for y = now_y, y < BH, ++y ; if not moveable(now_x, y, now) ; break ; end ; landed = y ; end
    now_y = landed ; lock_piece()
end

function ghost_y()
    for y = now_y, y < BH, ++y ; if not moveable(now_x, y, now) ; return y - 1 ; end ; end
    return BH - 1
end

function hold_piece()
    if store_used ; return ; end
    if not has_store ; store_id = now_id ; has_store = true ; new_piece()
    else
        var nb = clone(block_bag[store_id]) ; var nx = to_integer((BW - nb[0].size) / 2)
        if not moveable(nx, 0, nb) ; return ; end
        var tmp_id = store_id ; store_id = now_id ; now_id = tmp_id
        now = nb ; now_x = nx ; now_y = 0
    end
    store_used = true
end

# AI
function _eval_piece(b)
    # Evaluate a piece shape in all rotations and X positions, return best
    var best = {-999999, 0, 0}  # {score, rotation, x}
    for rot = 0, rot < 4, ++rot
        if rot > 0
            var ls = new array ; var rows = b[0].size ; var cols = b.size
            for i = 0, i < rows, ++i
                ls[i] = new array
                for j = 0, j < cols, ++j ; ls[i][j] = b[cols - j - 1][i] ; end
            end
            b = ls
        end
        for x = -2, x < BW, ++x
            if moveable(x, 0, b)
            var y = 0 ; for dy = 0, dy < BH, ++dy ; if not moveable(x, dy, b) ; break ; end ; y = dy ; end
            var sim = new array
            for i = 0, i < BH, ++i ; sim[i] = new array ; for j = 0, j < BW, ++j ; sim[i][j] = board[i][j] ; end ; end
            for i = 0, i < b.size, ++i
                for j = 0, j < b[0].size, ++j
                    if b[i][j] != 0 ; var by = i + y ; var bx = j + x ; if by >= 0 and by < BH ; sim[by][bx] = b[i][j] ; end ; end
                end
            end
            var cleared = 0 ; var ri = BH - 1
            while ri >= 0
                var full = true ; for c = 0, c < BW, ++c ; if sim[ri][c] == 0 ; full = false ; break ; end ; end
                if full
                    cleared = cleared + 1
                    for rr = ri, rr > 0, --rr ; for c = 0, c < BW, ++c ; sim[rr][c] = sim[rr - 1][c] ; end ; end
                    for c = 0, c < BW, ++c ; sim[0][c] = 0 ; end
                else ; ri = ri - 1 ; end
            end
            var heights = new array
            for c = 0, c < BW, ++c ; var h = BH ; for r = 0, r < BH, ++r ; if sim[r][c] != 0 ; h = r ; break ; end ; end ; heights[c] = BH - h ; end
            var ah = 0 ; for c = 0, c < BW, ++c ; ah = ah + heights[c] ; end
            var holes = 0
            for c = 0, c < BW, ++c ; var fb = false ; for r = 0, r < BH, ++r ; if sim[r][c] != 0 ; fb = true ; else ; if fb ; holes = holes + 1 ; end ; end ; end ; end
            var bump = 0 ; for c = 0, c < BW - 1, ++c ; bump = bump + math.abs(heights[c] - heights[c + 1]) ; end
            var s = -0.51 * ah - 4.0 * holes + 5.0 * cleared * cleared - 0.3 * bump
            if s > best[0] ; best = {s, rot, x} ; end
            end
        end
    end
    return best
end

function ai_evaluate()
    # Returns {best_rot, best_x, best_hold}
    var best_score = -999999 ; var best_rot = 0 ; var best_x = 0 ; var best_hold = false
    # Evaluate current piece
    var r = _eval_piece(clone(now))
    if r[0] > best_score ; best_score = r[0] ; best_rot = r[1] ; best_x = r[2] ; best_hold = false ; end
    # Evaluate hold piece (if we can hold)
    if not store_used
        var hold_id = next_id
        if has_store ; hold_id = store_id ; end
        var r2 = _eval_piece(clone(block_bag[hold_id]))
        if r2[0] > best_score ; best_score = r2[0] ; best_rot = r2[1] ; best_x = r2[2] ; best_hold = true ; end
    end
    return {best_rot, best_x, best_hold}
end

var ai_hold_done = false

function ai_step()
    if ai_target == null or game_over ; ai_target = ai_evaluate() ; ai_rot_done = false ; ai_hold_done = false ; end
    # Hold first if AI decided to
    if ai_target[2] and not ai_hold_done
        var saved_id = now_id
        hold_piece()
        ai_hold_done = true
        if now_id == saved_id
            # Hold failed, re-evaluate without hold
            ai_target = ai_evaluate() ; ai_rot_done = false ; ai_hold_done = false
        end
        return
    end
    if not ai_rot_done
        if ai_target[0] > 0
            spin_piece()
            ai_target[0] = ai_target[0] - 1
            if ai_target[0] == 0 ; ai_rot_done = true ; end
        else ; ai_rot_done = true ; end
        return
    end
    # Move toward target X, or drop if blocked / already there
    var moved = false
    if now_x < ai_target[1] and moveable(now_x + 1, now_y, now)
        now_x = now_x + 1 ; moved = true
    end
    if now_x > ai_target[1] and moveable(now_x - 1, now_y, now)
        now_x = now_x - 1 ; moved = true
    end
    if not moved
        hard_drop() ; ai_target = null ; ai_rot_done = false ; ai_hold_done = false
    end
end

# ========== Drawing ==========

function draw_board_bg()
    fill_rect(BOARD_X - 3, BOARD_Y - 3, BOARD_W + 6, BOARD_H + 6, C_SURFACE, 6)
    draw_rect(BOARD_X - 3, BOARD_Y - 3, BOARD_W + 6, BOARD_H + 6, C_BORDER, 1.5, 6)
    # Grid lines
    for i = 1, i < BW, ++i
        var x = BOARD_X + i * CS
        draw_line(x, BOARD_Y, x, BOARD_Y + BOARD_H, alpha(C_BORDER, 0.2), 0.5)
    end
    for j = 1, j < BH, ++j
        var y = BOARD_Y + j * CS
        draw_line(BOARD_X, y, BOARD_X + BOARD_W, y, alpha(C_BORDER, 0.2), 0.5)
    end
end

function draw_cell_3d(x, y, id)
    var cols = piece_colors(id)
    var fill_c = cols[0] ; var light_c = cols[1] ; var dark_c = cols[2]
    var cx = BOARD_X + x * CS + CELL_PAD
    var cy = BOARD_Y + y * CS + CELL_PAD
    var cw = CS - CELL_PAD * 2
    # Main fill
    fill_rect(cx, cy, cw, cw, fill_c, 3)
    # Light edge (top + left)
    fill_rect(cx, cy, cw, 3, light_c, 1)
    fill_rect(cx, cy, 3, cw, light_c, 1)
    # Dark edge (bottom + right)
    fill_rect(cx, cy + cw - 3, cw, 3, dark_c, 1)
    fill_rect(cx + cw - 3, cy, 3, cw, dark_c, 1)
end

function draw_ghost_cell(x, y, id)
    var cols = piece_colors(id)
    var cx = BOARD_X + x * CS + CELL_PAD
    var cy = BOARD_Y + y * CS + CELL_PAD
    var cw = CS - CELL_PAD * 2
    fill_rect(cx, cy, cw, cw, alpha(cols[0], 0.15), 3)
    draw_rect(cx, cy, cw, cw, alpha(cols[0], 0.4), 1.5, 3)
end

function draw_preview_cell(px, py, id)
    var cols = piece_colors(id)
    var ps = 16 ; var pd = 2
    var cx = px + pd ; var cy = py + pd ; var cw = ps - pd * 2
    fill_rect(cx, cy, cw, cw, cols[0], 2)
    fill_rect(cx, cy, cw, 2, cols[1], 1)
    fill_rect(cx, cy, 2, cw, cols[1], 1)
    fill_rect(cx, cy + cw - 2, cw, 2, cols[2], 1)
    fill_rect(cx + cw - 2, cy, 2, cw, cols[2], 1)
end

function draw_board_cells()
    for i = 0, i < BH, ++i
        for j = 0, j < BW, ++j
            if board[i][j] != 0 ; draw_cell_3d(j, i, board[i][j]) ; end
        end
    end
end

function draw_ghost()
    if game_over ; return ; end
    var gy = ghost_y()
    if gy <= now_y ; return ; end
    for i = 0, i < now.size, ++i
        for j = 0, j < now[0].size, ++j
            if now[i][j] != 0 ; draw_ghost_cell(now_x + j, gy + i, now[i][j]) ; end
        end
    end
end

function draw_piece()
    if game_over ; return ; end
    for i = 0, i < now.size, ++i
        for j = 0, j < now[0].size, ++j
            if now[i][j] != 0
                var y = now_y + i
                if y >= 0 ; draw_cell_3d(now_x + j, y, now[i][j]) ; end
            end
        end
    end
end

function draw_preview_block(px, py, title, bid)
    fill_rect(px - 4, py - 4, 72, 80, C_SURFACE, 4)
    draw_text(px, py - 16, title, C_GRAY, 11)
    var block = block_bag[bid]
    var off_x = px + to_integer((64 - block[0].size * 16) / 2)
    var off_y = py + to_integer((64 - block.size * 16) / 2)
    for i = 0, i < block.size, ++i
        for j = 0, j < block[0].size, ++j
            if block[i][j] != 0 ; draw_preview_cell(off_x + j * 16, off_y + i * 16, block[i][j]) ; end
        end
    end
end

function draw_info_panel()
    var px = PANEL_X ; var py = 118
    var panel_h = to_integer(text_height(11) * 9 + text_height(20) * 3 + text_height(16) * 2 + 46)
    fill_rect(px - 4, py - 4, 72, panel_h, C_SURFACE, 4)
    var ly = py + 2
    draw_text(px, ly, "SCORE", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px, ly, to_string(score), C_WHITE, 20) ; ly = ly + text_height(20) + 6
    draw_text(px, ly, "LINES", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px, ly, to_string(lines), C_CYAN, 20) ; ly = ly + text_height(20) + 6
    draw_text(px, ly, "LEVEL", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px, ly, to_string(level), C_YELLOW, 20) ; ly = ly + text_height(20) + 6
    var spd = to_integer(base_drop * 1000)
    draw_text(px, ly, "SPEED", C_GRAY, 11) ; ly = ly + text_height(11) + 3
    draw_text(px, ly, to_string(spd) + "ms", C_GRAY, 16) ; ly = ly + text_height(16) + 6
    var ac = C_GRAY ; if auto_play ; ac = C_GREEN ; end
    draw_text(px, ly, "AUTO", ac, 16)
end

function draw_bottom_bar()
    var by = BOARD_Y + BOARD_H + 8
    var bar_h = to_integer(text_height(11) * 2 + 20)
    fill_rect(BOARD_X - 3, by, BOARD_W + 6, bar_h, C_SURFACE, 4)
    var t1 = "W/Up:Spin  A/D/LR:Move  S/Down:Drop  Space:HardDrop  C:Hold"
    var t2 = "4/5:Speed  P:Auto  R:Reset"
    var ly = by + 8
    draw_text(BOARD_X + to_integer((BOARD_W - text_width(t1, 11)) / 2), ly, t1, C_GRAY, 11)
    ly = ly + text_height(11) + 4
    draw_text(BOARD_X + to_integer((BOARD_W - text_width(t2, 11)) / 2), ly, t2, C_GRAY, 11)
end

function draw_game_over()
    fill_rect(BOARD_X - 3, BOARD_Y - 3, BOARD_W + 6, BOARD_H + 6, alpha(C_BLACK, 0.75), 6)
    var cx = BOARD_X + BOARD_W / 2.0 ; var cy = BOARD_Y + BOARD_H / 2.0
    var t1 = "GAME OVER" ; var t2 = "Score: " + to_string(score) ; var t3 = "Press R to Restart"
    var total_h = text_height(26) + text_height(18) + text_height(13) + 16
    var y = cy - total_h / 2
    draw_text(cx - text_width(t1, 26) / 2, y, t1, C_RED, 26) ; y = y + text_height(26) + 10
    draw_text(cx - text_width(t2, 18) / 2, y, t2, C_WHITE, 18) ; y = y + text_height(18) + 6
    draw_text(cx - text_width(t3, 13) / 2, y, t3, C_GRAY, 13)
end

# ========== Main Loop ==========
init("Tetris", 500, 640)
init_game()

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.3 ; dt = 0.3 ; end

    if key_pressed(K_R) ; init_game() ; end
    if key_pressed(K_P) ; auto_play = not auto_play ; ai_target = null ; ai_rot_done = false ; end
    if not game_over
        if key_pressed(K_A) or key_pressed(K_LEFT) ; if not auto_play ; move_left() ; end ; end
        if key_pressed(K_D) or key_pressed(K_RIGHT) ; if not auto_play ; move_right() ; end ; end
        if key_pressed(K_W) or key_pressed(K_UP) ; if not auto_play ; spin_piece() ; end ; end
        if key_down(K_S) or key_down(K_DOWN) ; if not auto_play ; soft_drop() ; end ; end
        if key_pressed(K_SPACE) ; if not auto_play ; hard_drop() ; end ; end
        if key_pressed(K_C) ; if not auto_play ; hold_piece() ; end ; end
        if key_pressed(K_4) and base_drop < 5.0 ; base_drop = base_drop + 0.05 ; end
        if key_pressed(K_5) and base_drop > 0.05 ; base_drop = base_drop - 0.05 ; end
        if auto_play ; ai_step() ; end
        if not auto_play
            drop_timer = drop_timer + dt
            while drop_timer >= drop_intv
                drop_timer = drop_timer - drop_intv
                if not soft_drop() ; lock_piece() ; end
            end
        end
    end

    draw_board_bg()
    draw_board_cells()
    draw_ghost()
    draw_piece()
    if has_store ; draw_preview_block(HOLD_X, HOLD_Y, "HOLD", store_id) ; end
    draw_preview_block(PANEL_X, 30, "NEXT", next_id)
    draw_info_panel()
    draw_bottom_bar()
    if game_over ; draw_game_over() ; end
    end()
end
