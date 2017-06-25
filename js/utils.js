
// Converts from pixel coordinates to bitmap coordinates
function bitmap_position(i) {
    return Math.floor(i/block_size)
}

// Convert from bitmap coordinates to pixel coordinates
function pixel_position(i) {
    return i*block_size;
}

function create_2D_array(rows, cols) {
    var array = new Array(rows);
    for (var i = 0; i < rows; i++)
        array[i] = new Array(cols);
    return array;
}

// Draws a rectangle with (x, y) being the top left corner coordinates and 'w' and 'h' being the width and height respectively
function draw_block(x, y, width, height) {
    // Fill block
    context.beginPath();
    context.rect(x, y, width, height);
    context.closePath();
    context.fill();

    // Draw borders
    context.stroke();
}

function fetch_sprite (sprite_name) {
    switch(sprite_name) {
        case "vertical_walk_1":
            return [0, 0, 15, 22];
            break;
        case "vertical_walk_2":
            return [15, 0, 15, 22];
            break;
        case "vertical_walk_3":
            return [29, 0, 15, 22];
            break;
        case "vertical_walk_4":
            return [44, 0, 15, 22];
            break;
        case "vertical_walk_5":
            return [59, 0, 15, 22];
            break;
        case "vertical_walk_6":
            return [74, 0, 15, 22];
            break;
        case "horizontal_walk_1":
            return [89, 0, 15, 22];
            break;
        case "horizontal_walk_2":
            return [103, 0, 15, 22];
            break;
        case "horizontal_walk_3":
            return [117, 0, 15, 22];
            break;
        case "horizontal_walk_4":
            return [131, 0, 15, 22];
            break;
        case "horizontal_walk_5":
            return [145, 0, 15, 22];
            break;
        case "horizontal_walk_6":
            return [159, 0, 15, 22];
            break;
        case "bomb_small":
            return [0, 0, 16, 17];
            break;
        case "bomb_medium":
            return [19, 0, 16, 17];
            break;
        case "bomb_large":
            return [39, 0, 16, 17];
            break;
        case "explosion_middle":
            return [36, 36, 16, 16];
            break;
        case "explosion_middle_left":
            return [18, 36, 16, 16];
            break;
        case "explosion_extreme_left":
            return [0, 36, 16, 16];
            break;
        case "explosion_middle_right":
            return [54, 36, 16, 16];
            break;
        case "explosion_extreme_right":
            return [72, 36, 16, 16];
            break;
        case "explosion_middle_up":
            return [36, 18, 16, 16];
            break;
        case "explosion_extreme_up":
            return [36, 0, 16, 16];
            break;
        case "explosion_middle_down":
            return [36, 54, 16, 16];
            break;
        case "explosion_extreme_down":
            return [36, 72, 16, 16];
            break;
        case "power_glove":
            return [0, 0, 16, 16];
            break;
        case "boxing_glove":
            return [18, 0, 16, 16];
            break;
        case "fire":
            return [36, 0, 16, 16];
            break;
        case "skate":
            return [54, 0, 16, 16];
            break;
        case "line_bomb":
            return [72, 0, 16, 16];
            break;
        case "vest":
            return [90, 0, 16, 16];
            break;
        case "skull":
            return [108, 0, 16, 16];
            break;
        case "geta":
            return [126, 0, 16, 16];
            break;
        case "kick":
            return [144, 0, 16, 16];
            break;
        case "bomb":
            return [162, 0, 16, 16];
            break;
        default:
            console.log("Sprite name invalid");
            break;
    }
}
