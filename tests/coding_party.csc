# https://github.com/ocornut/imgui/issues/3606
import imgui
using  imgui

function FX(a, b, sz, mouse, t)
    # TODO
end

function FxTestBed(FX)
    var app=window_application(320, 180, "CovScript ImGUI Canvas")
    var p0 = vec2(0, 0), p1 = vec2(app.get_window_width(), app.get_window_height())
    var window_opened = true
    while !app.is_closed()
        app.prepare()
        begin_window("Main", window_opened, {flags.no_collapse, flags.no_title_bar, flags.no_move, flags.no_resize})
            if !window_opened
                break
            end
            set_window_pos(vec2(0,0))
            set_window_size(vec2(app.get_window_width(),app.get_window_height()))
            var mouse_data = vec4((get_mouse_pos_x() - p0.x) / p1.x, (get_mouse_pos_y() - p0.y) / p1.y, 0, 0)
            FX(p0, p1, p1, mouse_data, get_time())
        end_window()
        app.render()
    end
end

function IM_COL32(r, g, b, a)
    return vec4(r/255, g/255, b/255, a/255)
end

function FX1(a, b, sz, mouse, t)
    for (n = 0, n < (1.0 + math.sin(t * 5.7)) * 40.0, n++)
        add_circle(vec2(a.x + sz.x * 0.5, a.y + sz.y * 0.5), sz.y * (0.01 + n * 0.03), IM_COL32(255, 140 - n * 4, n * 3, 255), 360, 1);
    end
end

function FX2(_a, b, s, m, t)
    var a = vec2(_a.x + s.x/2, _a.y + s.y/2)
	var S = math.sin(m.x), C = math.cos(m.x), x = 50, y = 0, z = (m.y * 2 - 1) * x;
	var v = {{ x, x, z+x }, { x, -x, z+x }, { -x, -x, z+x }, { -x, x, z+x }, { x, x, z-x }, { x, -x, z-x }, { -x, -x, z-x }, { -x, x, z-x }}
	for (i = 0, i < 8, i++)
		x = v[i][0] * C - v[i][1] * S
		y = v[i][0] * S + v[i][1] * C + 120
		z = v[i][2]
		v[i][0] = x / y * 50
		v[i][1] = z / y * 50
		v[i][2] = y
	end
    function L(A, B)
        z = 500/(v[A][2] + v[B][2])
        add_line(vec2(a.x + v[A][0], a.y + v[A][1]), vec2(a.x + v[B][0], a.y + v[B][1]), vec4(1, 1, 1, 1), z);
    end
	L(0, 1); L(1, 2); L(2, 3); L(0, 3)
	L(4, 5); L(5, 6); L(6, 7); L(4, 7)
	L(0, 4); L(1, 5); L(2, 6); L(3, 7)
end

function FX3_rand()
    constant RAND_MAX = 32767
    return math.randint(0, RAND_MAX)
end

function FX3_v(N)
    var r = new array
    foreach i in range(N)
        var v = vec2(FX3_rand() % 320, FX3_rand() % 180)
        r.push_back(v : v)
    end
    return move(r)
end

function vec2_add(a, b)
    return vec2(a.x + b.x, a.y + b.y)
end

function vec2_sub(a, b)
    return vec2(a.x - b.x, a.y - b.y)
end

function vec2_div(a, b)
    return vec2(a.x/b, a.y/b)
end

function FX3(a, b, s, mouse, t)
    var l2 = [](x)->x.x*x.x + x.y*x.y
	var N = 300, D = 0, T = 0
    var v = FX3_v(N)
	foreach p in v
		D = math.sqrt(l2(vec2_sub(p.first, p.second)))
		if (D > 0)
            p.first = vec2_add(p.first, vec2_div(vec2_sub(p.second, p.first), D))
        end
		if (D < 4)
            p.second = vec2(FX3_rand() % 320, FX3_rand() % 180)
        end
	end
	for (i = 0, i < N, i++)
		for (j = i + 1, j < N, j++)
			D = l2(vec2_sub(v[i].first, v[j].first))
			T = l2(vec2_sub(vec2_add(v[i].first, v[j].first), s)) / 200
			if (T > 255)
                T = 255
            end
			if (D < 400)
                add_line(vec2_add(a, v[i].first), vec2_add(a, v[j].first), IM_COL32(T, 255-T, 255, 70), 2)
            end
		end
	end
end

function FX4(a, b, sz, mouse, t)
    var HP = [](p, s, c, o)->vec2(math.cos(p * math.constants.pi / 3 + o) * s + c.x, math.sin(p * math.constants.pi / 3 + o) * s + c.y)
    var SQ = [](t)->math.sin(t) + math.sin(3*t)/3 + math.sin(5*t)/5 + math.sin(7*t)/7
    var CI = [](i)->(to_integer(math.sin(i) * 128 + 128)) % 256
    var Z = 35.0
    for (x=-2, x<=5, x++)
        for (y = 0, y <= 4, y++)
            var o = vec2(a.x + math.sqrt(3) * Z * (x + y/2), a.y + y * 3 * Z / 2 + math.max(50*((4+(to_integer(y+x))%5)-(15-15/t)),0))
            for (s = 0, s <= Z, s += 5)
                var h = math.max(0, t < 2.5 ? 0 : SQ(t - 1.5 + x * math.min(t-2.5,1) + s * 10))
                var l = 0
                for (p = 1, p <= 6, p++)
                    add_line(HP(p % 6, s, o, h + math.constants.pi / 6), HP(l % 6, s, o, h + math.constants.pi / 6), IM_COL32(CI(h), CI(h + 2), CI(h + 3), 255), 1)
                    l = p
                end
            end
        end
    end
end

FxTestBed(context.solve(context.build(context.cmd_args.at(1))))