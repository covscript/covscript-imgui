# Tower of Hanoi (汉诺塔) — vivaldi 2D Engine
# 1/2/3/Click:Select  4/5:Speed  P:Auto  6:Disks  R:Reset

import vivaldi
using vivaldi

# ========== Layout ==========
constant PEG_Y_TOP = 70
constant PEG_Y_BOT = 340
constant PEG_H = PEG_Y_BOT - PEG_Y_TOP
constant PEG_W = 10
constant BASE_Y = PEG_Y_BOT
constant BASE_H = 14
constant BASE_W = 680
constant BASE_X = 60
constant DISK_H = 22
constant DISK_MIN_W = 40
constant DISK_MAX_W = 180

# ========== Game State ==========
var pegs = {new array, new array, new array}
var disk_count = 5
var selected = -1
var moves = 0
var won = false
var message = ""
var msg_timer = 0.0
var auto_play = false
var auto_moves = new array
var auto_idx = 0
var auto_timer = 0.0
var auto_speed = 0.5

function min_moves(n) ; var r = 1 ; for i = 0, i < n, ++i ; r = r * 2 ; end ; return r - 1 ; end

function peg_x(i)
    if i == 0 ; return 130 ; end
    if i == 1 ; return 400 ; end
    return 670
end

function init_game(n)
    if n == null ; n = disk_count ; end
    disk_count = n
    if disk_count < 3 ; disk_count = 3 ; end
    if disk_count > 8 ; disk_count = 8 ; end
    pegs = {new array, new array, new array}
    for i = disk_count - 1, i >= 0, --i ; pegs[0].push_back(i) ; end
    selected = -1 ; moves = 0 ; won = false
    message = "" ; msg_timer = 0.0
    auto_play = false ; auto_moves.clear() ; auto_idx = 0 ; auto_timer = 0.0
end

function gen_moves(n, frm, to, via, lst)
    if n == 1 ; lst.push_back({frm, to}) ; return ; end
    gen_moves(n - 1, frm, via, to, lst)
    lst.push_back({frm, to})
    gen_moves(n - 1, via, to, frm, lst)
end

function start_auto()
    init_game(disk_count)
    gen_moves(disk_count, 0, 2, 1, auto_moves)
    auto_idx = 0 ; auto_timer = 0.0 ; auto_play = true
end

function try_move(src, dst)
    if pegs[src].size == 0 ; message = "Source peg is empty!" ; msg_timer = 1.5 ; return false ; end
    var top_src = pegs[src][pegs[src].size - 1]
    if pegs[dst].size > 0
        var top_dst = pegs[dst][pegs[dst].size - 1]
        if top_src > top_dst ; message = "Cannot place larger disk on smaller disk!" ; msg_timer = 2.0 ; return false ; end
    end
    var disk = pegs[src][pegs[src].size - 1] ; pegs[src].pop_back() ; pegs[dst].push_back(disk)
    moves = moves + 1 ; message = "" ; msg_timer = 0.0 ; return true
end

function check_win()
    if pegs[1].size == disk_count or pegs[2].size == disk_count ; won = true ; auto_play = false ; end
end

function disk_color(si)
    if si == 0 ; return {0.9, 0.25, 0.3, 1.0} ; end
    if si == 1 ; return {0.95, 0.5, 0.15, 1.0} ; end
    if si == 2 ; return {0.95, 0.8, 0.15, 1.0} ; end
    if si == 3 ; return {0.18, 0.8, 0.35, 1.0} ; end
    if si == 4 ; return {0.1, 0.85, 0.85, 1.0} ; end
    if si == 5 ; return {0.25, 0.5, 0.95, 1.0} ; end
    if si == 6 ; return {0.75, 0.3, 0.75, 1.0} ; end
    return {0.65, 0.2, 0.8, 1.0}
end

function find_peg_at(mx, my)
    for i = 0, i < 3, ++i
        var px = peg_x(i) ; var hr = 70
        if mx >= px - hr and mx <= px + hr and my >= PEG_Y_TOP and my <= PEG_Y_BOT + 30 ; return i ; end
    end
    return -1
end

# ========== Drawing ==========

function draw_top_bar()
    fill_rect(20, 12, 760, 42, C_SURFACE, 6)
    var total = 2 ^ disk_count - 1
    var info = " Moves: " + to_string(moves) + "/" + to_string(total)
    info = info + "   Disks: " + to_string(disk_count)
    info = info + "   Speed: " + to_string(to_integer(auto_speed * 1000)) + "ms"
    draw_text(32, 24, info, C_WHITE, 14)
    var atxt = "AUTO: OFF"
    if auto_play ; atxt = "AUTO: ON" ; end
    var ac = C_GRAY ; if auto_play ; ac = C_GREEN ; end
    draw_text(660, 24, atxt, ac, 15)
end

function draw_pegs()
    for i = 0, i < 3, ++i
        var px = peg_x(i)
        # Peg shadow
        fill_rect(px - PEG_W/2 + 2, PEG_Y_TOP + 2, PEG_W, PEG_H, alpha(C_BLACK, 0.3), 4)
        # Peg
        fill_rect(px - PEG_W/2, PEG_Y_TOP, PEG_W, PEG_H, C_GRAY, 4)
        # Peg top knob
        fill_circle(px, PEG_Y_TOP, PEG_W/2 + 2, C_GRAY)
    end
    # Base
    fill_rect(BASE_X, BASE_Y + 4, BASE_W, BASE_H, alpha(C_BLACK, 0.3), 8)
    fill_rect(BASE_X, BASE_Y, BASE_W, BASE_H, C_GRAY, 8)
    # Base highlight line
    draw_line(BASE_X + 10, BASE_Y + 3, BASE_X + BASE_W - 10, BASE_Y + 3, alpha(C_WHITE, 0.15), 1)
end

function draw_disks()
    var dw_range = DISK_MAX_W - DISK_MIN_W
    for p = 0, p < 3, ++p
        var px = peg_x(p)
        for d = 0, d < pegs[p].size, ++d
            var si = pegs[p][d]
            var dw = DISK_MIN_W + to_integer(dw_range * (si + 1) / disk_count)
            if dw % 2 != 0 ; dw = dw + 1 ; end
            var dx = px - dw / 2
            var dy = BASE_Y - (d + 1) * DISK_H
            var col = disk_color(si)
            # Disk shadow
            fill_rect(dx + 2, dy + 2, dw, DISK_H, alpha(C_BLACK, 0.3), 6)
            # Disk body
            fill_rect(dx, dy, dw, DISK_H, col, 6)
            # Disk highlight
            fill_rect(dx + 4, dy + 3, dw - 8, 5, alpha(C_WHITE, 0.18), 3)
        end
    end
    # Selected highlight
    if selected >= 0 and pegs[selected].size > 0
        var px = peg_x(selected)
        var si = pegs[selected][pegs[selected].size - 1]
        var dw = DISK_MIN_W + to_integer(dw_range * (si + 1) / disk_count)
        if dw % 2 != 0 ; dw = dw + 1 ; end
        var dx = px - dw / 2
        var dy = BASE_Y - pegs[selected].size * DISK_H
        draw_rect(dx - 2, dy - 2, dw + 4, DISK_H + 4, C_GOLD, 3, 8)
    end
end

function draw_peg_labels()
    for i = 0, i < 3, ++i
        var px = peg_x(i)
        var txt = to_string(i + 1)
        draw_text(px - text_width(txt, 14) / 2, BASE_Y + BASE_H + 10, txt, C_GRAY, 14)
    end
end

function draw_bottom_bar()
    var bar_h = to_integer(text_height(12) + 16)
    fill_rect(20, 470, 760, bar_h, C_SURFACE, 4)
    var txt = "1/2/3:Select  Click:Peg  4/5:Speed  P:Auto  6:Disks  R:Reset"
    draw_text(to_integer((800 - text_width(txt, 12)) / 2), 470 + to_integer((bar_h - text_height(12)) / 2), txt, C_GRAY, 12)
end

function draw_message()
    if msg_timer > 0
        var y = PEG_Y_BOT + BASE_H + 32
        draw_text(to_integer((800 - text_width(message, 15)) / 2), y, message, C_RED, 15)
    end
end

function draw_win()
    var t1 = "Congratulations!" ; var t2 = "Solved in " + to_string(moves) + " moves"
    var t3 = "Minimum: " + to_string(min_moves(disk_count))
    var total_h = text_height(28) + text_height(18) + text_height(14) + 20
    var by = 200 - total_h / 2 ; var bw = 500.0 ; var bx = (800 - bw) / 2
    fill_rect(bx, by - 10, bw, total_h + 20, alpha(C_GREEN, 0.85), 12)
    draw_rect(bx, by - 10, bw, total_h + 20, C_WHITE, 2, 12)
    var y = by + 6
    draw_text(to_integer((800 - text_width(t1, 28)) / 2), y, t1, C_WHITE, 28) ; y = y + text_height(28) + 8
    draw_text(to_integer((800 - text_width(t2, 18)) / 2), y, t2, C_WHITE, 18) ; y = y + text_height(18) + 6
    draw_text(to_integer((800 - text_width(t3, 14)) / 2), y, t3, alpha(C_WHITE, 0.7), 14)
end

# ========== Main Loop ==========
init("Tower of Hanoi", 800, 520)
init_game(disk_count)

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.2 ; dt = 0.2 ; end

    if key_pressed(K_R) ; init_game(disk_count) ; end
    if key_pressed(K_P)
        if won ; init_game(disk_count) ; end
        if not auto_play ; start_auto() ; else ; auto_play = false ; end
    end
    if key_pressed(K_4) and auto_speed < 3.0 ; auto_speed = auto_speed + 0.05 ; end
    if key_pressed(K_5) and auto_speed > 0.05 ; auto_speed = auto_speed - 0.05 ; end
    if key_pressed(K_6) ; var nd = disk_count + 1 ; if nd > 8 ; nd = 3 ; end ; init_game(nd) ; end

    if msg_timer > 0 ; msg_timer = msg_timer - dt ; if msg_timer < 0 ; msg_timer = 0 ; message = "" ; end ; end

    if not won
        if auto_play
            auto_timer = auto_timer + dt
            while auto_timer >= auto_speed and auto_idx < auto_moves.size
                auto_timer = auto_timer - auto_speed
                var mv = auto_moves[auto_idx] ; try_move(mv[0], mv[1]) ; check_win() ; auto_idx = auto_idx + 1
                if won ; break ; end
            end
            if auto_idx >= auto_moves.size and not won ; auto_play = false ; end
        end

        # Keyboard selection
        if key_pressed(K_1)
            if selected < 0 ; if pegs[0].size > 0 ; selected = 0 ; end
            else ; if selected != 0 ; try_move(selected, 0) ; check_win() ; selected = -1 ; else ; selected = -1 ; end ; end
        end
        if key_pressed(K_2)
            if selected < 0 ; if pegs[1].size > 0 ; selected = 1 ; end
            else ; if selected != 1 ; try_move(selected, 1) ; check_win() ; selected = -1 ; else ; selected = -1 ; end ; end
        end
        if key_pressed(K_3)
            if selected < 0 ; if pegs[2].size > 0 ; selected = 2 ; end
            else ; if selected != 2 ; try_move(selected, 2) ; check_win() ; selected = -1 ; else ; selected = -1 ; end ; end
        end

        # Mouse selection
        if mouse_clicked(0)
            var peg = find_peg_at(mouse_x(), mouse_y())
            if peg >= 0
                if selected < 0
                    if pegs[peg].size > 0 ; selected = peg ; end
                else
                    if selected != peg ; try_move(selected, peg) ; check_win() ; selected = -1
                    else ; selected = -1 ; end
                end
            end
        end
    end

    fill_rect(0, 0, 800, 520, C_DARK, 0)
    draw_top_bar()
    draw_pegs()
    draw_disks()
    draw_peg_labels()
    draw_bottom_bar()
    draw_message()
    if won ; draw_win() ; end
    end()
end
