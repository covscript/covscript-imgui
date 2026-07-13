# Breakout (打砖块) — vivaldi 2D Engine
# A/D:Move  Space:Launch  4/5:Speed  P:Auto  R:Reset

import vivaldi
using vivaldi

# ========== Layout ==========
constant WW = 600.0 ; constant WH = 700.0
constant PAD_W = 100.0 ; constant PAD_H = 16.0 ; constant PAD_Y = WH - 60
constant BALL_R = 9.0
constant BRICK_COLS = 10 ; constant BRICK_ROWS = 6
constant BRICK_W = 52.0 ; constant BRICK_H = 20.0 ; constant BRICK_GAP = 4.0
constant GRID_LEFT = (WW - BRICK_COLS * (BRICK_W + BRICK_GAP)) / 2
constant GRID_TOP = 60.0
constant PAD_SPEED = 550.0
constant BALL_SPEED = 420.0
constant MAX_LIVES = 3

# ========== Game State ==========
var pad_x = WW / 2 - PAD_W / 2
var ball_x = WW / 2 ; var ball_y = PAD_Y - BALL_R - 4
var ball_vx = 0.0 ; var ball_vy = 0.0
var ball_attached = true
var bricks = null
var score = 0 ; var lives = MAX_LIVES
var game_over = false ; var won = false
var auto_play = false
var base_speed = 1.0
var particles = null

function brick_color(row)
    if row == 0 ; return C_RED ; end
    if row == 1 ; return C_ORANGE ; end
    if row == 2 ; return C_YELLOW ; end
    if row == 3 ; return C_GREEN ; end
    if row == 4 ; return C_CYAN ; end
    return C_BLUE
end

function init_bricks()
    bricks = new array
    for r = 0, r < BRICK_ROWS, ++r
        bricks[r] = new array
        for c = 0, c < BRICK_COLS, ++c ; bricks[r][c] = 1 ; end
    end
end

function brick_left(c) ; return GRID_LEFT + c * (BRICK_W + BRICK_GAP) ; end
function brick_top(r)  ; return GRID_TOP + r * (BRICK_H + BRICK_GAP) ; end

function init_game()
    pad_x = WW / 2 - PAD_W / 2
    ball_x = WW / 2 ; ball_y = PAD_Y - BALL_R - 4 ; ball_vx = 0 ; ball_vy = 0
    ball_attached = true
    init_bricks()
    score = 0 ; lives = MAX_LIVES ; game_over = false ; won = false
    base_speed = 1.0 ; auto_play = false
    particles = particles_new(300)
end

function launch_ball()
    if not ball_attached ; return ; end
    ball_attached = false
    ball_vx = random_float(-120, 120)
    ball_vy = -BALL_SPEED
end

function spawn_brick_particles(cx, cy, col)
    for i = 0, i < 12, ++i
        var vx = random_float(-200, 200) ; var vy = random_float(-200, 50)
        particles_emit(particles, cx, cy, vx, vy, random_float(0.3, 0.8), col, random_float(3, 6))
    end
end

function ball_hits_brick()
    for r = 0, r < BRICK_ROWS, ++r
        for c = 0, c < BRICK_COLS, ++c
            if bricks[r][c] != 0
                var bx = brick_left(c) ; var by = brick_top(r)
                if rect_hit(ball_x - BALL_R, ball_y - BALL_R, BALL_R*2, BALL_R*2, bx, by, BRICK_W, BRICK_H)
                    bricks[r][c] = 0 ; score = score + (BRICK_ROWS - r) * 10
                    var cx = bx + BRICK_W / 2 ; var cy = by + BRICK_H / 2
                    spawn_brick_particles(cx, cy, brick_color(r))
                    var overlap_left = ball_x + BALL_R - bx
                    var overlap_right = bx + BRICK_W - (ball_x - BALL_R)
                    var overlap_top = ball_y + BALL_R - by
                    var overlap_bottom = by + BRICK_H - (ball_y - BALL_R)
                    var min_x = overlap_left ; if overlap_right < min_x ; min_x = overlap_right ; end
                    var min_y = overlap_top ; if overlap_bottom < min_y ; min_y = overlap_bottom ; end
                    if min_x < min_y
                        if overlap_left <= overlap_right and ball_vx > 0 ; ball_vx = -ball_vx ; ball_x = bx - BALL_R ; end
                        if overlap_right < overlap_left and ball_vx < 0 ; ball_vx = -ball_vx ; ball_x = bx + BRICK_W + BALL_R ; end
                    else
                        if overlap_top <= overlap_bottom and ball_vy > 0 ; ball_vy = -ball_vy ; ball_y = by - BALL_R ; end
                        if overlap_bottom < overlap_top and ball_vy < 0 ; ball_vy = -ball_vy ; ball_y = by + BRICK_H + BALL_R ; end
                    end
                    return
                end
            end
        end
    end
end

function all_bricks_cleared()
    for r = 0, r < BRICK_ROWS, ++r
        for c = 0, c < BRICK_COLS, ++c ; if bricks[r][c] != 0 ; return false ; end ; end
    end
    return true
end

function update(dt)
    if game_over ; return ; end
    # Ball attached to paddle
    if ball_attached
        ball_x = pad_x + PAD_W / 2
        return
    end
    # Move ball
    ball_x = ball_x + ball_vx * base_speed * dt
    ball_y = ball_y + ball_vy * base_speed * dt
    # Walls
    if ball_x - BALL_R < 0 ; ball_x = BALL_R ; ball_vx = -ball_vx ; end
    if ball_x + BALL_R > WW ; ball_x = WW - BALL_R ; ball_vx = -ball_vx ; end
    if ball_y - BALL_R < 0 ; ball_y = BALL_R ; ball_vy = -ball_vy ; end
    # Bottom
    if ball_y + BALL_R > WH
        lives = lives - 1
        if lives <= 0 ; game_over = true ; return ; end
        ball_attached = true ; ball_x = pad_x + PAD_W / 2 ; ball_y = PAD_Y - BALL_R - 4
        ball_vx = 0 ; ball_vy = 0
        return
    end
    # Paddle
    if ball_vy > 0 and ball_y + BALL_R >= PAD_Y and ball_y - BALL_R <= PAD_Y + PAD_H
        if ball_x + BALL_R > pad_x and ball_x - BALL_R < pad_x + PAD_W
            ball_y = PAD_Y - BALL_R
            var hit_pos = (ball_x - pad_x) / PAD_W - 0.5
            ball_vx = hit_pos * BALL_SPEED * 1.5
            ball_vy = -BALL_SPEED * 0.8
            var spd = length(ball_vx, ball_vy)
            if spd > 0 ; ball_vx = ball_vx / spd * BALL_SPEED ; ball_vy = ball_vy / spd * BALL_SPEED ; end
            base_speed = base_speed + 0.01 ; if base_speed > 2.5 ; base_speed = 2.5 ; end
        end
    end
    # Bricks
    ball_hits_brick()
    # Win check
    if all_bricks_cleared() ; game_over = true ; won = true ; end
    particles_update(particles, dt)
end

# ========== Drawing ==========

function draw_bricks()
    for r = 0, r < BRICK_ROWS, ++r
        for c = 0, c < BRICK_COLS, ++c
            if bricks[r][c] != 0
                var bx = brick_left(c) ; var by = brick_top(r)
                fill_rect(bx, by, BRICK_W, BRICK_H, brick_color(r), 4)
                fill_rect(bx + 2, by + 2, BRICK_W - 4, 4, alpha(C_WHITE, 0.25), 2)
            end
        end
    end
end

function draw_paddle()
    fill_rect(pad_x, PAD_Y, PAD_W, PAD_H, C_WHITE, 6)
    fill_rect(pad_x + 4, PAD_Y + 2, PAD_W - 8, 4, alpha(C_WHITE, 0.5), 2)
end

function draw_ball()
    fill_circle(ball_x, ball_y, BALL_R, C_WHITE)
    fill_circle(ball_x - 2, ball_y - 2, 3, alpha(C_WHITE, 0.6))
end

function draw_hud()
    var bar_h = to_integer(text_height(16) + 20)
    fill_rect(10, 10, WW - 20, bar_h, C_SURFACE, 6)
    var sc = "Score: " + to_string(score)
    draw_text(24, 10 + to_integer((bar_h - text_height(16)) / 2), sc, C_WHITE, 16)
    var li = "Lives: "
    var lw = text_width(li, 16)
    draw_text(WW - 180, 10 + to_integer((bar_h - text_height(16)) / 2), li, C_GRAY, 16)
    for i = 0, i < lives, ++i ; fill_circle(WW - 180 + lw + 16 + i * 28, 10 + bar_h / 2, 10, C_RED) ; end
    var sp = "Spd: " + to_string(to_integer(base_speed * 100)) + "%"
    draw_text(WW/2 - text_width(sp, 14) / 2, 10 + to_integer((bar_h - text_height(14)) / 2), sp, C_CYAN, 14)
end

function draw_bottom_bar()
    var by = WH - 34 ; var bar_h = to_integer(text_height(12) + 14)
    fill_rect(20, by, WW - 40, bar_h, C_SURFACE, 4)
    var txt = "A/D:Move  Space:Launch  4/5:Speed  P:Auto  R:Reset"
    draw_text(to_integer((WW - text_width(txt, 12)) / 2), by + to_integer((bar_h - text_height(12)) / 2), txt, C_GRAY, 12)
end

function draw_game_over()
    fill_rect(WW/2 - 200, WH/2 - 50, 400, 100, alpha(C_BLACK, 0.85), 10)
    draw_rect(WW/2 - 200, WH/2 - 50, 400, 100, C_BORDER, 2, 10)
    if won
        var t1 = "You Win!"
        draw_text(WW/2 - text_width(t1, 32) / 2, WH/2 - 30, t1, C_GREEN, 32)
    else
        var t1 = "Game Over"
        draw_text(WW/2 - text_width(t1, 32) / 2, WH/2 - 30, t1, C_RED, 32)
    end
    var t2 = "Score: " + to_string(score) + "   Press R to Restart"
    draw_text(WW/2 - text_width(t2, 15) / 2, WH/2 + 14, t2, C_WHITE, 15)
end

# ========== Main Loop ==========
init("Breakout", to_integer(WW), to_integer(WH))
init_game()

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.05 ; dt = 0.05 ; end

    if key_pressed(K_R) ; init_game() ; end
    if key_pressed(K_P) ; auto_play = not auto_play ; end
    if key_pressed(K_SPACE) ; launch_ball() ; end
    if key_pressed(K_4) and base_speed < 3.0 ; base_speed = base_speed + 0.1 ; end
    if key_pressed(K_5) and base_speed > 0.5 ; base_speed = base_speed - 0.1 ; end
    if not game_over
        if key_down(K_A) or key_down(K_LEFT) ; pad_x = pad_x - PAD_SPEED * dt ; end
        if key_down(K_D) or key_down(K_RIGHT) ; pad_x = pad_x + PAD_SPEED * dt ; end
        if pad_x < 0 ; pad_x = 0 ; end
        if pad_x > WW - PAD_W ; pad_x = WW - PAD_W ; end
        if auto_play
            if ball_attached ; launch_ball() ; end
            # Add randomness to target so AI doesn't hit same column forever
            var jitter = random_float(-PAD_W * 0.3, PAD_W * 0.3)
            var target = ball_x - PAD_W / 2 + jitter
            var diff = target - pad_x
            var mx = PAD_SPEED * 1.1 * dt
            if diff > mx ; diff = mx ; end ; if diff < -mx ; diff = -mx ; end
            pad_x = pad_x + diff
        end
        update(dt)
    end

    fill_rect(0, 0, WW, WH, C_DARK, 0)
    draw_bricks()
    particles_draw(particles)
    draw_paddle()
    draw_ball()
    draw_hud()
    draw_bottom_bar()
    if game_over ; draw_game_over() ; end
    end()
end
