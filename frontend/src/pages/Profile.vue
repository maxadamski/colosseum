<script>
export default {
    name: 'Profile',
    data: () => ({
        nick: 'Max',
        group: 'I9-2020',
    }),
}
</script>

<template lang="pug">
div
    h3 User Profile

    h4 Nickname
    .hlist
        input(type='text' :value='nick')
        button Save

    h4 Group
    .hlist
        .select
            select
                option I8-2020
                option I9-2020
        button Save

    h3 Game-Maker Zone

    h4 My Games
    table.w-50
        tr
            th Game
            th Actions
        tr
            td Pentago
            td.hlist
                button Edit
                button Reset
        tr
            td Chess
            td.hlist
                button Edit
                button Reset
                button Activate
        tr
            td New game
            td
                router-link(to='/edit-game')
                    button Create

    h4 My Groups
    table.w-50
        tr
            th Name
            th Actions
        tr
            td I8-2020
            td.hlist
                button Export
                button Rename
                button Delete
        tr
            td I9-2020
            td.hlist
                button Export
                button Rename
                button Delete
        tr
            td New group
            td
                button Create
</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"
</style>
