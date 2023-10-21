<?php

if (!get_role(ROLE_COACH)) {
    add_role(
            ROLE_COACH,
            'Edző',
            ['read' => true,]
    );
}
if (!get_role(ROLE_PLAYER)) {
    add_role(
            ROLE_PLAYER,
            'Játékos',
            ['read' => true,]
    );
}
