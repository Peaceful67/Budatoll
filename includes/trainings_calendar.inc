<?php

function budatoll_my_trainings_calendar() {
    return budatoll_trainings_calendar(get_current_user_id());
}

function budatoll_trainings_calendar($player_id = -1) {
   
    return 'Training calendar of user: ' . $player_id;
}
