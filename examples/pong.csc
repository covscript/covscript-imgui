# Pong (乒乓) — vivaldi 2D Engine
# W/S:Move  4/5:Speed  P:Auto  R:Reset

import vivaldi
using vivaldi

# ========== Layout ==========
constant WW = 800.0 ; constant WH = 500.0
constant PAD_W = 16.0 ; constant PAD_H = 90.0
constant BALL_R = 10.0
constant PAD_SPEED = 500.0
constant BALL_SPEED = 380.0
constant WIN_SCORE = 7

# ========== Game State ==========
var pad1_y = WH / 2 - PAD_H / 2   # left (AI)
var pad2_y = WH / 2 - PAD_H / 2   # right (player)
var ball_x = WW / 2 ; var ball_y = WH / 2
var ball_vx = BALL_SPEED ; var ball_vy = 0.0
var score1 = 0 ; var score2 = 0
var game_over = false ; var winner = 0
var auto_play = false
var base_speed = 1.0
var particles = null

function reset_ball(dir)
    ball_x = WW / 2 ; ball_y = WH / 2
    ball_vx = BALL_SPEED * dir
    ball_vy = random_float(-150, 150)
end

function init_game()
    pad1_y = WH / 2 - PAD_H / 2 ; pad2_y = WH / 2 - PAD_H / 2
    score1 = 0 ; score2 = 0 ; game_over = false ; winner = 0
    base_speed = 1.0 ; auto_play = false
    particles = particles_new(200)
    reset_ball(1)
end

function ai_move(dt)
    # Track ball with intentional lag for natural feel
    var target = ball_y - PAD_H / 2
    if ball_vx < 0 ; target = target + random_float(-20, 20) ; end
    if ball_vx > 0 ; target = WH / 2 - PAD_H / 2 + random_float(-30, 30) ; end
    var diff = target - pad1_y
    var max_move = PAD_SPEED * 0.7 * dt
    if ball_vx < 0 ; max_move = PAD_SPEED * 0.95 * dt ; end
    if diff > max_move ; diff = max_move ; end
    if diff < -max_move ; diff = -max_move ; end
    pad1_y = pad1_y + diff
    if pad1_y < 10 ; pad1_y = 10 ; end
    if pad1_y > WH - PAD_H - 10 ; pad1_y = WH - PAD_H - 10 ; end
end

function spawn_particles(x, y, count)
    for i = 0, i < count, ++i
        var vx = random_float(-300, 300) ; var vy = random_float(-300, 300)
        particles_emit(particles, x, y, vx, vy, random_float(0.3, 0.7), C_WHITE, random_float(2, 5))
    end
end

function update(dt)
    if game_over ; return ; end
    if not auto_play ; ai_move(dt) ; else ; pad1_y = clamp(ball_y - PAD_H / 2, 10, WH - PAD_H - 10) ; end
    ball_x = ball_x + ball_vx * base_speed * dt
    ball_y = ball_y + ball_vy * base_speed * dt
    if ball_y - BALL_R < 0 ; ball_y = BALL_R ; ball_vy = -ball_vy ; spawn_particles(ball_x, ball_y, 8) ; end
    if ball_y + BALL_R > WH ; ball_y = WH - BALL_R ; ball_vy = -ball_vy ; spawn_particles(ball_x, ball_y, 8) ; end
    # Left paddle
    if ball_x - BALL_R < 30 and ball_x + BALL_R > 30 - PAD_W
        if ball_y + BALL_R > pad1_y and ball_y - BALL_R < pad1_y + PAD_H
            ball_x = 30 + BALL_R
            var hit_pos = (ball_y - pad1_y) / PAD_H - 0.5
            ball_vx = BALL_SPEED ; ball_vy = hit_pos * BALL_SPEED * 2.0
            if ball_vy < 80 and ball_vy > -80 ; if hit_pos >= 0 ; ball_vy = 80 ; else ; ball_vy = -80 ; end ; end
            var spd = length(ball_vx, ball_vy) ; ball_vx = ball_vx / spd * BALL_SPEED ; ball_vy = ball_vy / spd * BALL_SPEED
            base_speed = base_speed + 0.03
            spawn_particles(ball_x, ball_y, 15)
        end
    end
    # Right paddle
    if ball_x + BALL_R > WW - 30 and ball_x - BALL_R < WW - 30 + PAD_W
        if ball_y + BALL_R > pad2_y and ball_y - BALL_R < pad2_y + PAD_H
            ball_x = WW - 30 - BALL_R
            var hit_pos = (ball_y - pad2_y) / PAD_H - 0.5
            ball_vx = -BALL_SPEED ; ball_vy = hit_pos * BALL_SPEED * 2.0
            if ball_vy < 80 and ball_vy > -80 ; if hit_pos >= 0 ; ball_vy = 80 ; else ; ball_vy = -80 ; end ; end
            var spd = length(ball_vx, ball_vy) ; ball_vx = ball_vx / spd * BALL_SPEED ; ball_vy = ball_vy / spd * BALL_SPEED
            base_speed = base_speed + 0.03
            spawn_particles(ball_x, ball_y, 15)
        end
    end
    if ball_x < -BALL_R ; score2 = score2 + 1 ; reset_ball(1) ; if score2 >= WIN_SCORE ; game_over = true ; winner = 2 ; end ; end
    if ball_x > WW + BALL_R ; score1 = score1 + 1 ; reset_ball(-1) ; if score1 >= WIN_SCORE ; game_over = true ; winner = 1 ; end ; end
    if auto_play
        var t2 = ball_y - PAD_H / 2 ; var d2 = t2 - pad2_y ; var mx = PAD_SPEED * dt
        if d2 > mx ; d2 = mx ; end ; if d2 < -mx ; d2 = -mx ; end
        pad2_y = pad2_y + d2
        if pad2_y < 10 ; pad2_y = 10 ; end
        if pad2_y > WH - PAD_H - 10 ; pad2_y = WH - PAD_H - 10 ; end
    end
    particles_update(particles, dt)
end

# ========== Drawing ==========

function draw_field()
    fill_rect(0, 0, WW, WH, C_DARK, 0)
    for y = 20, y < WH - 20, y = y + 30 ; fill_rect(WW/2 - 2, y, 4, 16, alpha(C_WHITE, 0.2), 1) ; end
end

function draw_paddles()
    fill_rect(30 - PAD_W, pad1_y, PAD_W, PAD_H, C_WHITE, 4)
    fill_rect(WW - 30, pad2_y, PAD_W, PAD_H, C_WHITE, 4)
end

function draw_ball()
    fill_circle(ball_x, ball_y, BALL_R, C_WHITE)
    fill_circle(ball_x - 2, ball_y - 2, 3, alpha(C_WHITE, 0.5))
end

function draw_score()
    var s1 = to_string(score1) ; var s2 = to_string(score2)
    draw_text(WW/2 - 70 - text_width(s1, 48), 20, s1, alpha(C_WHITE, 0.4), 48)
    draw_text(WW/2 + 70, 20, s2, alpha(C_WHITE, 0.4), 48)
    if score1 >= WIN_SCORE ; draw_text(WW/2 - 70 - text_width(s1, 48), 20, s1, C_GREEN, 48) ; end
    if score2 >= WIN_SCORE ; draw_text(WW/2 + 70, 20, s2, C_GREEN, 48) ; end
end

function draw_game_over()
    fill_rect(WW/2 - 200, WH/2 - 40, 400, 80, alpha(C_BLACK, 0.8), 10)
    draw_rect(WW/2 - 200, WH/2 - 40, 400, 80, C_BORDER, 2, 10)
    var txt = "Player " + to_string(winner) + " Wins!"
    draw_text(WW/2 - text_width(txt, 30) / 2, WH/2 - 20, txt, C_GOLD, 30)
    var t2 = "Press R to Restart"
    draw_text(WW/2 - text_width(t2, 14) / 2, WH/2 + 22, t2, C_GRAY, 14)
end

function draw_bottom_bar()
    var by = WH - 34 ; var bar_h = to_integer(text_height(12) + 14)
    fill_rect(20, by, WW - 40, bar_h, C_SURFACE, 4)
    var txt = "W/S:Move  4/5:Speed  P:Auto  R:Reset"
    draw_text(to_integer((WW - text_width(txt, 12)) / 2), by + to_integer((bar_h - text_height(12)) / 2), txt, C_GRAY, 12)
end

# ========== Main Loop ==========
init("Pong", to_integer(WW), to_integer(WH))
init_game()

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.05 ; dt = 0.05 ; end

    if key_pressed(K_R) ; init_game() ; end
    if key_pressed(K_P) ; auto_play = not auto_play ; end
    if key_pressed(K_4) and base_speed < 3.0 ; base_speed = base_speed + 0.05 ; end
    if key_pressed(K_5) and base_speed > 0.5 ; base_speed = base_speed - 0.05 ; end
    if not auto_play and not game_over
        if key_down(K_W) or key_down(K_UP) ; pad2_y = pad2_y - PAD_SPEED * dt ; end
        if key_down(K_S) or key_down(K_DOWN) ; pad2_y = pad2_y + PAD_SPEED * dt ; end
        if pad2_y < 10 ; pad2_y = 10 ; end
        if pad2_y > WH - PAD_H - 10 ; pad2_y = WH - PAD_H - 10 ; end
    end

    update(dt)
    draw_field()
    particles_draw(particles)
    draw_paddles()
    draw_ball()
    draw_score()
    draw_bottom_bar()
    if game_over ; draw_game_over() ; end
    end()
end
