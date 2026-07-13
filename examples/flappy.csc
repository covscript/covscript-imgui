# Flappy Bird 1:1 — vivaldi 2D Engine
# Space/W/Up:Flap  4/5:Speed  P:Auto  R:Reset

import vivaldi
using vivaldi

# ========== Layout & Original Colors ==========
constant WW = 400.0 ; constant WH = 650.0
constant BIRD_X = 100.0 ; constant BIRD_R = 14.0
constant GRAVITY = 1000.0 ; constant FLAP_VEL = -360.0
constant PIPE_W = 52.0 ; constant PIPE_GAP = 140.0
constant PIPE_SPEED = 150.0
constant GROUND_H = 80.0 ; constant GROUND_Y = WH - GROUND_H

# Original Flappy Bird palette
constant SKY_TOP    = {0.45, 0.77, 0.81, 1.0}   # #73C5CE
constant SKY_BOT    = {0.31, 0.75, 0.79, 1.0}   # #4EC0CA
constant BIRD_BODY  = {0.97, 0.77, 0.23, 1.0}   # #F8C53A
constant BIRD_WING  = {0.86, 0.60, 0.13, 1.0}   # #DC9820
constant BIRD_BEAK  = {0.91, 0.45, 0.14, 1.0}   # #E87424
constant PIPE_GREEN = {0.45, 0.75, 0.18, 1.0}   # #74BF2E
constant PIPE_DARK  = {0.33, 0.56, 0.14, 1.0}   # #548F24
constant PIPE_LIGHT = {0.55, 0.83, 0.27, 1.0}   # #8CD644
constant GROUND_COLOR = {0.87, 0.85, 0.58, 1.0} # #DED895
constant GRASS_COLOR  = {0.37, 0.70, 0.20, 1.0} # #5EB234
constant CLOUD_COLOR  = {0.88, 0.94, 0.95, 1.0}

# ========== Game State ==========
var bird_y = WH / 2 ; var bird_vy = 0.0
var pipes = null
var score = 0
var game_over = false ; var started = false
var auto_play = false ; var base_speed = 1.0
var pipe_timer = 0.0
var clouds = null     # array of {x, y, w, h}
var ground_offset = 0.0
var best_score = 0 ; var is_new_best = false

function init_game()
    bird_y = WH / 2 ; bird_vy = 0.0 ; pipes = new array
    score = 0 ; game_over = false ; started = false
    base_speed = 1.0 ; auto_play = false ; pipe_timer = 0.0
    ground_offset = 0.0 ; is_new_best = false
    # Generate clouds
    clouds = new array
    for i = 0, i < 5, ++i
        clouds.push_back({random_float(0, WW), random_float(20, 200), random_float(50, 90), random_float(25, 40)})
    end
end

function flap()
    if game_over ; return ; end
    started = true ; bird_vy = FLAP_VEL
end

function spawn_pipe()
    var gap_cy = random_float(PIPE_GAP + 40, GROUND_Y - PIPE_GAP - 40)
    if gap_cy < PIPE_GAP/2 + 30 ; gap_cy = PIPE_GAP/2 + 30 ; end
    if gap_cy > GROUND_Y - PIPE_GAP/2 - 30 ; gap_cy = GROUND_Y - PIPE_GAP/2 - 30 ; end
    pipes.push_back({WW + PIPE_W, gap_cy, false})
end

function update(dt)
    if game_over ; return ; end
    if not started ; return ; end
    # Clouds
    for i = 0, i < clouds.size, ++i ; clouds[i][0] = clouds[i][0] - 30 * dt ; if clouds[i][0] < -120 ; clouds[i][0] = WW + 60 ; end ; end
    # Ground scroll (synced with pipes)
    ground_offset = ground_offset + PIPE_SPEED * base_speed * dt
    # Bird
    bird_vy = bird_vy + GRAVITY * dt
    bird_y = bird_y + bird_vy * dt
    if bird_y + BIRD_R > GROUND_Y ; bird_y = GROUND_Y - BIRD_R ; game_over = true ; if score > best_score ; best_score = score ; is_new_best = true ; end ; end
    if bird_y - BIRD_R < 0 ; bird_y = BIRD_R ; bird_vy = 0 ; game_over = true ; if score > best_score ; best_score = score ; is_new_best = true ; end ; end
    # Pipes
    pipe_timer = pipe_timer + dt
    var interval = PIPE_GAP * 1.6 / (PIPE_SPEED * base_speed)
    if pipe_timer >= interval or pipes.size == 0 ; pipe_timer = 0 ; spawn_pipe() ; end
    for i = 0, i < pipes.size, ++i
        var px = pipes[i][0] - PIPE_SPEED * base_speed * dt
        var gap_cy = pipes[i][1] ; var scored = pipes[i][2]
        if not scored and px + PIPE_W/2 < BIRD_X ; scored = true ; score = score + 1 ; end
        var top_bot = gap_cy - PIPE_GAP / 2 ; var bot_top = gap_cy + PIPE_GAP / 2
        if BIRD_X + BIRD_R - 2 > px and BIRD_X - BIRD_R + 2 < px + PIPE_W
            if bird_y - BIRD_R + 2 < top_bot or bird_y + BIRD_R - 2 > bot_top ; game_over = true ; if score > best_score ; best_score = score ; is_new_best = true ; end ; end
        end
        pipes[i] = {px, gap_cy, scored}
    end
    while pipes.size > 0 and pipes[0][0] < -PIPE_W - 10 ; pipes.pop_front() ; end
end

# ========== Drawing ==========

function draw_sky()
    var bands = 64 ; var bh = GROUND_Y / bands
    for i = 0, i < bands, ++i
        var t = i / (bands - 1.0)
        var r = SKY_TOP[0]*(1-t)+SKY_BOT[0]*t
        var g = SKY_TOP[1]*(1-t)+SKY_BOT[1]*t
        var b = SKY_TOP[2]*(1-t)+SKY_BOT[2]*t
        fill_rect(0, i * bh, WW, bh + 1, {r,g,b,1.0}, 0)
    end
end

function draw_clouds()
    for i = 0, i < clouds.size, ++i
        var cx = clouds[i][0] ; var cy = clouds[i][1] ; var cw = clouds[i][2] ; var ch = clouds[i][3]
        fill_circle(cx, cy, ch * 0.6, CLOUD_COLOR)
        fill_circle(cx - cw * 0.25, cy + ch * 0.15, ch * 0.5, CLOUD_COLOR)
        fill_circle(cx + cw * 0.25, cy + ch * 0.1, ch * 0.55, CLOUD_COLOR)
        fill_circle(cx + cw * 0.1, cy - ch * 0.1, ch * 0.45, CLOUD_COLOR)
    end
end

function draw_pipes()
    for i = 0, i < pipes.size, ++i
        var x = pipes[i][0] ; var gap_cy = pipes[i][1]
        var top_h = gap_cy - PIPE_GAP / 2 ; var bot_y = gap_cy + PIPE_GAP / 2
        # Top pipe body
        fill_rect(x - 2, 0, PIPE_W + 4, top_h - 16, PIPE_GREEN, 0)
        fill_rect(x, 0, PIPE_W, top_h - 16, PIPE_LIGHT, 0)
        # Top pipe cap
        fill_rect(x - 4, top_h - 18, PIPE_W + 8, 18, PIPE_DARK, 3)
        fill_rect(x - 2, top_h - 16, PIPE_W + 4, 16, PIPE_GREEN, 3)
        fill_rect(x - 2, top_h - 16, PIPE_W + 4, 4, PIPE_LIGHT, 2)
        # Bottom pipe body
        fill_rect(x - 2, bot_y + 2, PIPE_W + 4, WH - bot_y, PIPE_GREEN, 0)
        fill_rect(x, bot_y + 2, PIPE_W, WH - bot_y, PIPE_LIGHT, 0)
        # Bottom pipe cap
        fill_rect(x - 4, bot_y, PIPE_W + 8, 18, PIPE_DARK, 3)
        fill_rect(x - 2, bot_y + 2, PIPE_W + 4, 16, PIPE_GREEN, 3)
        fill_rect(x - 2, bot_y + 14, PIPE_W + 4, 4, PIPE_LIGHT, 2)
    end
end

function draw_ground()
    fill_rect(0, GROUND_Y, WW, GROUND_H, GROUND_COLOR, 0)
    fill_rect(0, GROUND_Y, WW, 12, GRASS_COLOR, 0)
    fill_rect(0, GROUND_Y + 3, WW, 4, alpha(C_BLACK, 0.08), 0)
    # Grass tufts (scroll synced with pipes)
    var t = to_integer(ground_offset) % 20
    for x = -t - 20, x < WW + 20, x = x + 20
        fill_rect(x, GROUND_Y - 6, 3, 8, GRASS_COLOR, 1)
        fill_rect(x + 10, GROUND_Y - 4, 2, 6, GRASS_COLOR, 1)
    end
end

function draw_bird()
    var tilt = clamp(bird_vy / 500.0, -0.5, 0.8)
    var bx = BIRD_X ; var by = bird_y
    # Body
    fill_circle(bx, by, BIRD_R, BIRD_BODY)
    fill_circle(bx, by + 2, BIRD_R * 0.85, BIRD_BODY)
    # Wing
    var wing_angle = tilt * 0.5
    if bird_vy < -100 ; wing_angle = -0.6 ; end
    if bird_vy > 200 ; wing_angle = 0.5 ; end
    var wx = bx - 5 + wing_angle * 8 ; var wy = by + 2
    fill_circle(wx, wy, 9, BIRD_WING)
    # Eye
    var ex = bx + BIRD_R * 0.4 + tilt * 2 ; var ey = by - 3
    fill_circle(ex, ey, 6, C_WHITE)
    fill_circle(ex + 2, ey + 0.5, 3, C_BLACK)
    # Beak
    fill_rect(bx + BIRD_R - 2, by + 1, 10, 5, BIRD_BEAK, 2)
    fill_rect(bx + BIRD_R + 2, by + 2, 6, 3, alpha(BIRD_BEAK, 0.7), 1)
end

function draw_score()
    var s = to_string(score)
    draw_text(WW/2 - text_width(s, 42)/2 + 2, 40 + 2, s, alpha(C_BLACK, 0.3), 42)
    draw_text(WW/2 - text_width(s, 42)/2, 40, s, C_WHITE, 42)
end

function draw_get_ready()
    if not started and not game_over
        var t = "TAP TO START"
        draw_text(WW/2 - text_width(t, 18)/2 + 1, WH/2 - 50 + 1, t, alpha(C_BLACK, 0.3), 18)
        draw_text(WW/2 - text_width(t, 18)/2, WH/2 - 50, t, C_WHITE, 18)
    end
end

function draw_game_over()
    var cx = WW / 2
    # Title
    var t1 = "GAME OVER"
    draw_text(cx - text_width(t1, 26)/2 + 2, WH/2 - 130 + 2, t1, alpha(C_BLACK, 0.3), 26)
    draw_text(cx - text_width(t1, 26)/2, WH/2 - 130, t1, C_WHITE, 26)
    # Medal
    var mx = cx ; var my = WH/2 - 80
    var mc = {0.80, 0.50, 0.20, 1.0}   # bronze
    var mc2 = {0.65, 0.38, 0.12, 1.0}
    if score >= 40 ; mc = {0.78, 0.88, 0.92, 1.0} ; mc2 = {0.55, 0.68, 0.75, 1.0}    ; end # platinum
    if score >= 30 and score < 40 ; mc = {0.95, 0.82, 0.15, 1.0} ; mc2 = {0.80, 0.65, 0.08, 1.0}  ; end # gold
    if score >= 20 and score < 30 ; mc = {0.82, 0.82, 0.82, 1.0} ; mc2 = {0.60, 0.60, 0.60, 1.0}  ; end # silver
    fill_circle(mx, my, 22, mc)
    fill_circle(mx, my, 17, alpha(C_WHITE, 0.25))
    fill_circle(mx, my, 14, mc2)
    if score >= 10
        fill_rect(mx - 2, my - 9, 4, 18, alpha(C_WHITE, 0.4), 1)
        fill_rect(mx - 9, my - 2, 18, 4, alpha(C_WHITE, 0.4), 1)
    end
    # Scoreboard panel
    var px = cx - 100 ; var py = WH/2 - 38 ; var pw = 200 ; var ph = 80
    fill_rect(px, py, pw, ph, {0.96, 0.92, 0.78, 0.95}, 6)
    draw_rect(px, py, pw, ph, alpha(C_BLACK, 0.25), 2, 6)
    # Score row
    draw_text(px + 16, py + 10, "SCORE", {0.45, 0.35, 0.20, 1.0}, 12)
    var ss = to_string(score)
    draw_text(px + pw - 16 - text_width(ss, 22), py + 8, ss, {0.2, 0.2, 0.2, 1.0}, 22)
    # Divider
    fill_rect(px + 14, py + 38, pw - 28, 1, alpha(C_BLACK, 0.12), 0)
    # Best row
    draw_text(px + 16, py + 46, "BEST", {0.45, 0.35, 0.20, 1.0}, 12)
    var bs = to_string(best_score)
    var bs_x = px + pw - 16 - text_width(bs, 22)
    if is_new_best
        bs_x = bs_x - 38
        fill_rect(bs_x - 4, py + 48, 30, 13, {0.90, 0.25, 0.30, 1.0}, 3)
        draw_text(bs_x, py + 49, "NEW", C_WHITE, 10)
        draw_text(bs_x + 34, py + 44, bs, {0.2, 0.2, 0.2, 1.0}, 22)
    else
        draw_text(bs_x, py + 44, bs, {0.2, 0.2, 0.2, 1.0}, 22)
    end
    # Restart hint
    var t3 = "Press Space or R to Restart"
    draw_text(cx - text_width(t3, 12)/2, py + ph + 16, t3, C_GRAY, 12)
end

# ========== Main Loop ==========
init("Flappy Bird", to_integer(WW), to_integer(WH))
init_game()

while not is_closed()
    begin()
    var dt = get_delta()
    if dt > 0.05 ; dt = 0.05 ; end

    if key_pressed(K_R) or (game_over and key_pressed(K_SPACE)) ; init_game() ; end
    if key_pressed(K_P) ; auto_play = not auto_play ; end
    if key_pressed(K_SPACE) or key_pressed(K_W) or key_pressed(K_UP) ; flap() ; end
    if key_pressed(K_4) and base_speed < 3.0 ; base_speed = base_speed + 0.1 ; end
    if key_pressed(K_5) and base_speed > 0.5 ; base_speed = base_speed - 0.1 ; end
    if auto_play
        if not started ; flap() ; end
        var gap_cy = WH / 2
        for i = 0, i < pipes.size, ++i
            if pipes[i][0] + PIPE_W/2 > BIRD_X ; gap_cy = pipes[i][1] ; break ; end
        end
        # Predict position after gravity for ~0.1s
        var py = bird_y + bird_vy * 0.1 + GRAVITY * 0.01 / 2
        # Flap if predicted to drop below gap center while not already rising fast
        if py > gap_cy and bird_vy > -200 ; flap() ; end
    end

    update(dt)
    draw_sky()
    draw_clouds()
    draw_pipes()
    draw_ground()
    draw_bird()
    draw_score()
    draw_get_ready()
    if game_over ; draw_game_over() ; end
    end()
end
