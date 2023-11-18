<?php

function budatoll_settings_page() {
    add_menu_page(
            'Budatoll beállítások',
            'Budatoll',
            'manage_options',
            'budatoll-settings',
            'budatoll_settings_content'
    );
}

function budatoll_settings_content() {
    echo '<div class="wrap">';
    echo '<h2>Budatoll beállítások</h2>';
    // Add your settings form here
    echo '</div>';
    echo '<form method="post" action="options.php">';
    settings_fields('my-plugin-settings-group');
    do_settings_sections('my-plugin-settings');
    echo '<input type="text" name="my_plugin_option" value="' . esc_attr(get_option('my_plugin_option')) . '">';
    submit_button();
    echo '</form>';
}

function budatoll_register_settings() {
    register_setting('budatoll-settings-group', 'budatoll_option');
}
add_action('admin_init', 'budatoll_register_settings');

function budatoll_function() {
   
}
add_action('init', 'budatoll_function');