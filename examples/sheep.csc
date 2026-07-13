# Sheep Happens! (羊了个羊) 1:1 — vivaldi 2D Engine
# Click:Select  1:Shuffle  2:Undo  3/4:Difficulty  R:Reset

import vivaldi
using vivaldi

# ========== Layout ==========
constant WW = 480.0 ; constant WH = 780.0
constant TILE_W = 52.0 ; constant TILE_H = 60.0
constant TILE_GAP = 4.0
constant SLOT_Y = WH - 140.0 ; var SLOT_COUNT = 7
constant TILE_TYPES = 10

# Colors
constant BG_GREEN  = {0.55, 0.78, 0.47, 1.0}
constant BG_YELLOW = {0.90, 0.85, 0.60, 1.0}
constant CARD_BG   = {0.97, 0.97, 0.95, 1.0}
constant CARD_DIM  = {0.65, 0.65, 0.63, 1.0}
constant SLOT_WOOD = {0.65, 0.50, 0.35, 1.0}

# ========== Tile type data ==========
function icon_color(tp)
    if tp == 0 ; return {0.92, 0.30, 0.25, 1.0} ; end
    if tp == 1 ; return {0.25, 0.55, 0.92, 1.0} ; end
    if tp == 2 ; return {0.22, 0.82, 0.35, 1.0} ; end
    if tp == 3 ; return {0.95, 0.72, 0.10, 1.0} ; end
    if tp == 4 ; return {0.78, 0.28, 0.82, 1.0} ; end
    if tp == 5 ; return {0.10, 0.82, 0.80, 1.0} ; end
    if tp == 6 ; return {0.95, 0.50, 0.55, 1.0} ; end
    if tp == 7 ; return {0.52, 0.72, 0.20, 1.0} ; end
    if tp == 8 ; return {0.68, 0.45, 0.22, 1.0} ; end
    return {0.55, 0.55, 0.55, 1.0}
end

function draw_icon(tp, cx, cy, sz)
    var c = icon_color(tp) ; var hsz = sz / 2.0 ; var q = hsz / 2.0
    if tp == 0   # Bell: circle + rect
        fill_circle(cx, cy - q, hsz * 0.6, c)
        fill_rect(cx - q, cy, sz * 0.5, q * 1.2, c, 3)
        fill_circle(cx, cy + q, q * 0.5, c)
    end
    if tp == 1   # Diamond: 4 circles
        fill_circle(cx, cy - q * 1.2, q * 0.8, c)
        fill_circle(cx, cy + q * 1.2, q * 0.8, c)
        fill_circle(cx - q * 1.2, cy, q * 0.8, c)
        fill_circle(cx + q * 1.2, cy, q * 0.8, c)
        fill_circle(cx, cy, q * 0.6, alpha(C_WHITE, 0.3))
    end
    if tp == 2   # Clover: 3 circles + stem
        fill_circle(cx, cy - q, q, c)
        fill_circle(cx - q, cy + q * 0.3, q, c)
        fill_circle(cx + q, cy + q * 0.3, q, c)
        fill_rect(cx - 2, cy + q * 0.5, 4, q, c, 1)
    end
    if tp == 3   # Coin: circle + inner ring
        fill_circle(cx, cy, hsz * 0.75, c)
        fill_circle(cx, cy, hsz * 0.45, alpha(C_WHITE, 0.3))
    end
    if tp == 4   # Heart: 2 circles + triangle
        fill_circle(cx - q, cy - q * 0.5, q * 0.8, c)
        fill_circle(cx + q, cy - q * 0.5, q * 0.8, c)
        fill_rect(cx - hsz * 0.75, cy - q * 0.2, hsz * 1.5, q * 1.2, c, 4)
    end
    if tp == 5   # Drop: circle + rect
        fill_circle(cx, cy - q * 0.6, q * 0.9, c)
        fill_rect(cx - q * 0.6, cy, q * 1.2, q, c, 4)
    end
    if tp == 6   # Flower: center + 4 petals
        fill_circle(cx - q, cy - q, q * 0.7, c)
        fill_circle(cx + q, cy - q, q * 0.7, c)
        fill_circle(cx - q, cy + q, q * 0.7, c)
        fill_circle(cx + q, cy + q, q * 0.7, c)
        fill_circle(cx, cy, q * 0.6, C_YELLOW)
    end
    if tp == 7   # Cross in diamond
        fill_rect(cx - hsz * 0.7, cy - hsz * 0.7, hsz * 1.4, hsz * 1.4, c, 4)
        draw_line(cx - q, cy - q, cx + q, cy + q, C_WHITE, 2.5)
        draw_line(cx + q, cy - q, cx - q, cy + q, C_WHITE, 2.5)
    end
    if tp == 8   # Arrow/chevron: 3 bars
        fill_rect(cx - hsz * 0.8, cy - 4, hsz * 1.6, 3, c, 1)
        fill_rect(cx - hsz * 0.5, cy + 4, hsz * 1.0, 3, c, 1)
        fill_rect(cx - hsz * 0.2, cy + 12, hsz * 0.4, 3, c, 1)
    end
    if tp == 9   # Moon: circle - circle
        fill_circle(cx - q * 0.3, cy, hsz * 0.75, c)
        fill_circle(cx + q * 0.5, cy - q * 0.2, hsz * 0.55, CARD_BG)
    end
    if tp == 10  # Ring: outer circle + inner hole
        fill_circle(cx, cy, hsz * 0.7, c)
        fill_circle(cx, cy, hsz * 0.35, CARD_BG)
    end
    if tp == 11  # Keyhole: circle + rect
        fill_circle(cx, cy - q * 0.3, q * 0.6, c)
        fill_rect(cx - q * 0.25, cy - q * 0.1, q * 0.5, hsz * 0.5, c, 2)
    end
end

# ========== Game State ==========
var tiles = null    # array of {type, cx, cy, layer, removed}
var slot = null     # array of types in slot
var history = null  # undo: array of {tile_idx}
var game_over = false ; var won = false
var difficulty = 1   # 0=Easy, 1=Normal, 2=Hard
var num_types = 8    # number of tile types (varies by difficulty)

function is_covered(idx)
    var t = tiles[idx]
    if t[4] ; return true ; end
    var tlx = t[1] - TILE_W/2 ; var tly = t[2] - TILE_H/2
    for i = 0, i < tiles.size, ++i
        var o = tiles[i]
        if not o[4] and o[3] > t[3]
            var olx = o[1] - TILE_W/2 ; var oly = o[2] - TILE_H/2
            if rect_hit(tlx + 4, tly + 4, TILE_W - 8, TILE_H - 8, olx + 4, oly + 4, TILE_W - 8, TILE_H - 8) ; return true ; end
        end
    end
    return false
end

function init_tiles()
    tiles = new array ; slot = new array ; history = new array
    game_over = false ; won = false
    # Difficulty: 0=Easy(2 layers), 1=Normal(3 layers), 2=Hard(3 layers+more types)
    var num_layers = 2 ; if difficulty >= 1 ; num_layers = 3 ; end
    num_types = 8 ; if difficulty == 1 ; num_types = 10 ; end ; if difficulty == 2 ; num_types = 12 ; end
    var slot_cap = 8 ; if difficulty == 1 ; slot_cap = 7 ; end ; if difficulty == 2 ; slot_cap = 6 ; end
    SLOT_COUNT = slot_cap
    # 3 layers: bottom big, middle medium, top small
    # Upper layers sit at the intersection of 4 lower tiles (1 presses 4)
    var layers = new array
    # Layer 0: 7×8 = 56
    layers.push_back({7, 8, 0, 36.0, 147.0})
    # Layer 1: 5×6 = 30, offset by half tile to sit on 4 intersections
    layers.push_back({5, 6, 1, 36.0 + (TILE_W + TILE_GAP) * 0.5, 147.0 + (TILE_H + TILE_GAP) * 0.5})
    # Layer 2: 3×4 = 12, offset again by half tile
    layers.push_back({3, 4, 2, 36.0 + (TILE_W + TILE_GAP), 147.0 + (TILE_H + TILE_GAP)})
    # Generate tiles ensuring each type appears in multiples of 3
    var all_types = new array
    for li = 0, li < num_layers, ++li
        var spec = layers[li]
        for r = 0, r < spec[0], ++r
            for c = 0, c < spec[1], ++c
                all_types.push_back(random_int(0, num_types - 1))
            end
        end
    end
    # Balance: make each type count multiple of 3
    var counts = new array
    for tp = 0, tp < num_types, ++tp ; counts.push_back(0) ; end
    for i = 0, i < all_types.size, ++i ; counts[all_types[i]] = counts[all_types[i]] + 1 ; end
    for tp = 0, tp < num_types, ++tp
        var extra = counts[tp] % 3
        if extra != 0
            var removed = 0
            for i = all_types.size - 1, i >= 0, --i
                if all_types[i] == tp and removed < extra ; all_types[i] = -1 ; removed = removed + 1 ; end
            end
        end
    end
    # Create tiles
    var ti = 0
    for li = 0, li < num_layers, ++li
        var spec = layers[li]
        for r = 0, r < spec[0], ++r
            for c = 0, c < spec[1], ++c
                if ti < all_types.size
                    var tp = all_types[ti] ; ti = ti + 1
                    if tp >= 0
                        var cx = spec[3] + c * (TILE_W + TILE_GAP)
                        var cy = spec[4] + r * (TILE_H + TILE_GAP)
                        tiles.push_back({tp, cx, cy, spec[2], false})
                    end
                end
            end
        end
    end
end

function shuffle_tiles()
    # Collect all non-removed tile types across all layers
    var all_types = new array
    for i = 0, i < tiles.size, ++i
        if not tiles[i][4] ; all_types.push_back(tiles[i][0]) ; end
    end
    # Fisher-Yates shuffle the combined type pool
    for i = all_types.size - 1, i > 0, --i
        var j = random_int(0, i)
        var tmp = all_types[i] ; all_types[i] = all_types[j] ; all_types[j] = tmp
    end
    # Reassign shuffled types back to tiles in order
    var ti = 0
    for i = 0, i < tiles.size, ++i
        if not tiles[i][4]
            tiles[i] = {all_types[ti], tiles[i][1], tiles[i][2], tiles[i][3], false}
            ti = ti + 1
        end
    end
    history.clear()
end

function check_match()
    var counts = new array
    for tp = 0, tp < num_types, ++tp ; counts.push_back(0) ; end
    for i = 0, i < slot.size, ++i ; counts[slot[i]] = counts[slot[i]] + 1 ; end
    # Keep only count%3 leftovers for each type
    var keep = new array
    for tp = 0, tp < num_types, ++tp ; keep.push_back(counts[tp] % 3) ; end
    var new_slot = new array
    for i = 0, i < slot.size, ++i
        var tp = slot[i]
        if keep[tp] > 0 ; new_slot.push_back(tp) ; keep[tp] = keep[tp] - 1 ; end
    end
    slot = new_slot
end

function click_tile(idx)
    if game_over or won ; return ; end
    if is_covered(idx) ; return ; end
    if slot.size >= SLOT_COUNT ; return ; end
    var tp = tiles[idx][0]
    tiles[idx] = {tp, tiles[idx][1], tiles[idx][2], tiles[idx][3], true}
    var old_slot = new array
    for i = 0, i < slot.size, ++i ; old_slot.push_back(slot[i]) ; end
    slot.push_back(tp)
    history.push_back({idx, old_slot})
    check_match()
    if slot.size >= SLOT_COUNT ; game_over = true ; end
    var remaining = 0
    for i = 0, i < tiles.size, ++i ; if not tiles[i][4] ; remaining = remaining + 1 ; end ; end
    if remaining == 0 ; won = true ; end
end

function undo_move()
    if history.size == 0 ; return ; end
    var h = history[history.size - 1] ; history.pop_back()
    var idx = h[0] ; var t = tiles[idx]
    tiles[idx] = {t[0], t[1], t[2], t[3], false}
    slot = h[1]
    game_over = false
end

function find_tile_at(mx, my)
    var best = -1 ; var best_layer = -1
    for i = 0, i < tiles.size, ++i
        var t = tiles[i]
        if not t[4] and not is_covered(i)
        if mx >= t[1] - TILE_W/2 and mx <= t[1] + TILE_W/2
            if my >= t[2] - TILE_H/2 and my <= t[2] + TILE_H/2
                if t[3] > best_layer ; best_layer = t[3] ; best = i ; end
            end
        end
        end
    end
    return best
end

# ========== Drawing ==========

function draw_background()
    fill_rect(0, 0, WW, WH * 0.6, BG_GREEN, 0)
    fill_rect(0, WH * 0.5, WW, WH * 0.5, BG_YELLOW, 0)
    # Ground line
    fill_rect(0, WH * 0.58, WW, 4, alpha(C_BLACK, 0.06), 0)
    # Grass tufts
    for x = 20, x < WW, x = x + 45 ; fill_rect(x, WH*0.5 - 10, 3, 12, BG_GREEN, 1) ; end
end

function draw_title()
    var diff_name = "Easy" ; if difficulty == 1 ; diff_name = "Normal" ; end ; if difficulty == 2 ; diff_name = "Hard" ; end
    fill_rect(WW/2 - 120, 10, 240, 44, alpha(C_BLACK, 0.08), 12)
    draw_text(WW/2 - text_width("Sheep Happens!", 22)/2 + 1, 19, "Sheep Happens!", alpha(C_BLACK, 0.2), 22)
    draw_text(WW/2 - text_width("Sheep Happens!", 22)/2, 18, "Sheep Happens!", C_WHITE, 22)
    draw_text(WW/2 - text_width(diff_name, 12)/2, 38, diff_name, C_GRAY, 12)
end

function draw_tiles()
    for i = 0, i < tiles.size, ++i
        var t = tiles[i]
        if not t[4]
        var covered = is_covered(i)
        var bg = CARD_BG ; if covered ; bg = CARD_DIM ; end
        var x = t[1] - TILE_W/2 ; var y = t[2] - TILE_H/2
        var layer = t[3]
        # Stack edges: draw offset cards behind to show depth
        if layer > 0 and not covered
            for s = layer, s > 0, --s
                var ox = s * 3 ; var oy = s * 3
                fill_rect(x + ox, y + oy, TILE_W, TILE_H, alpha(C_BLACK, 0.08 + s * 0.06), 6)
            end
        end
        # Shadow
        if not covered ; fill_rect(x + 3, y + 3, TILE_W, TILE_H, alpha(C_BLACK, 0.2), 6) ; end
        # Card
        fill_rect(x, y, TILE_W, TILE_H, bg, 6)
        if not covered ; fill_rect(x + 3, y + 3, TILE_W - 6, TILE_H/4, alpha(C_WHITE, 0.5), 4) ; end
        # Layer indicator dots
        if not covered and layer > 0
            for d = 0, d < layer, ++d
                fill_circle(x + 8 + d * 8, y + TILE_H - 8, 3, alpha(C_WHITE, 0.6))
            end
        end
        # Icon
        if not covered
            draw_icon(t[0], t[1], t[2] + 2, 22)
        else
            draw_icon(t[0], x + TILE_W/2, y + TILE_H/2 + 2, 22)
        end
        end
    end
end

function draw_slot()
    var slot_w = SLOT_COUNT * (TILE_W + 6)
    var sx = (WW - slot_w) / 2
    fill_rect(sx - 8, SLOT_Y - 8, slot_w + 16, TILE_H + 20, SLOT_WOOD, 10)
    draw_rect(sx - 8, SLOT_Y - 8, slot_w + 16, TILE_H + 20, alpha(C_BLACK, 0.2), 2, 10)
    for i = 0, i < SLOT_COUNT, ++i
        var x = sx + i * (TILE_W + 6)
        if i < slot.size
            var tp = slot[i]
            fill_rect(x, SLOT_Y, TILE_W, TILE_H, CARD_BG, 5)
            draw_icon(tp, x + TILE_W/2, SLOT_Y + TILE_H/2 + 2, 22)
        else
            draw_rect(x, SLOT_Y, TILE_W, TILE_H, alpha(C_BLACK, 0.12), 1, 4)
        end
    end
end

function draw_buttons()
    var by = SLOT_Y + TILE_H + 36
    var bw = 90.0 ; var bh = 32.0 ; var gap = 14.0
    var bx = (WW - (bw * 3 + gap * 2)) / 2
    # Shuffle
    fill_rect(bx, by, bw, bh, C_SURFACE, 6)
    draw_text(bx + bw/2 - text_width("Shuffle", 13)/2, by + bh/2 - text_height(13)/2, "Shuffle", C_WHITE, 13)
    # Undo
    fill_rect(bx + bw + gap, by, bw, bh, C_SURFACE, 6)
    draw_text(bx + bw + gap + bw/2 - text_width("Undo", 13)/2, by + bh/2 - text_height(13)/2, "Undo", C_WHITE, 13)
    # Reset
    fill_rect(bx + (bw + gap) * 2, by, bw, bh, C_SURFACE, 6)
    draw_text(bx + (bw + gap)*2 + bw/2 - text_width("Reset", 13)/2, by + bh/2 - text_height(13)/2, "Reset", C_WHITE, 13)
    # Labels
    draw_text(bx + bw/2, by - 16, "[1]", C_GRAY, 10)
    draw_text(bx + bw + gap + bw/2, by - 16, "[2]", C_GRAY, 10)
    draw_text(bx + (bw + gap)*2 + bw/2, by - 16, "[R]", C_GRAY, 10)
end

function draw_overlay()
    if game_over and not won
        fill_rect(0, 0, WW, WH, alpha(C_BLACK, 0.5), 0)
        fill_rect(WW/2 - 150, WH/2 - 50, 300, 100, C_SURFACE, 12)
        var t = "SLOT FULL"
        draw_text(WW/2 - text_width(t, 26)/2, WH/2 - 16, t, C_RED, 26)
        draw_text(WW/2 - text_width("Press R to Restart", 14)/2, WH/2 + 24, "Press R to Restart", C_GRAY, 14)
    end
    if won
        fill_rect(0, 0, WW, WH, alpha(C_BLACK, 0.5), 0)
        fill_rect(WW/2 - 150, WH/2 - 50, 300, 100, C_SURFACE, 12)
        var t = "ALL CLEAR!"
        draw_text(WW/2 - text_width(t, 28)/2, WH/2 - 16, t, C_GREEN, 28)
        draw_text(WW/2 - text_width("Press R to Restart", 14)/2, WH/2 + 24, "Press R to Restart", C_GRAY, 14)
    end
end

function find_button_at(mx, my)
    var by = SLOT_Y + TILE_H + 36
    var bw = 90.0 ; var bh = 32.0 ; var gap = 14.0
    var bx = (WW - (bw * 3 + gap * 2)) / 2
    if my >= by and my < by + bh
        if mx >= bx and mx < bx + bw ; return 1 ; end
        if mx >= bx + bw + gap and mx < bx + bw + gap + bw ; return 2 ; end
        if mx >= bx + (bw + gap) * 2 and mx < bx + (bw + gap) * 2 + bw ; return 3 ; end
    end
    return 0
end

# ========== Main Loop ==========
init("Sheep Happens!", to_integer(WW), to_integer(WH))
init_tiles()

while not is_closed()
    begin()

    if key_pressed(K_R) ; init_tiles() ; end
    if key_pressed(K_1) ; shuffle_tiles() ; end
    if key_pressed(K_2) ; undo_move() ; end
    if key_pressed(K_3) ; difficulty = difficulty - 1 ; if difficulty < 0 ; difficulty = 0 ; end ; init_tiles() ; end
    if key_pressed(K_4) ; difficulty = difficulty + 1 ; if difficulty > 2 ; difficulty = 2 ; end ; init_tiles() ; end
    if mouse_clicked(0)
        var btn = find_button_at(mouse_x(), mouse_y())
        if btn == 1 ; shuffle_tiles() ; end
        if btn == 2 ; undo_move() ; end
        if btn == 3 ; init_tiles() ; end
        if btn == 0 and not game_over and not won
            var idx = find_tile_at(mouse_x(), mouse_y())
            if idx >= 0 ; click_tile(idx) ; end
        end
    end

    draw_background()
    draw_title()
    draw_tiles()
    draw_slot()
    draw_buttons()
    draw_overlay()
    end()
end
