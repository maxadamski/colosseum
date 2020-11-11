<script>
export default {
    name: 'Profile',
    data: () => ({
        nick: 'Max',
        group: 'I9-2020',
        type: 'student'
    }),
}
</script>

<template lang="pug">
div
    h2 User Profile

    h3 Basic Information

    h4 Nickname
    .hcombo
        input(type='text' :value='nick')
        button Save


    div(v-if='type == "student"')
        h4 Group
        .hcombo
            .select
                select
                    option I8-2020
                    option I9-2020
            button Save

    div(v-if='type == "teacher"')
        h2 Game-Maker Zone

        h3 My Games
        
        .rounded.w-50
            table
                tr
                    th Game
                    th Actions
                tr
                    td Pentago
                    td.hcombo
                        button Edit
                        button Reset
                tr
                    td Chess
                    td.hcombo
                        button Edit
                        button Reset
                        button Activate
                tr
                    td New game
                    td.hcombo
                        router-link(to='/edit-game')
                            button Create

        h3 My Groups
        .rounded.w-50
            table
                tr
                    th Name
                    th Actions
                tr
                    td I8-2020
                    td.hcombo
                        button Export
                        button Rename
                        button Delete
                tr
                    td I9-2020
                    td.hcombo
                        button Export
                        button Rename
                        button Delete
                tr
                    td New group
                    td.hcombo
                        button Create
</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"
</style>
