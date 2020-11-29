<script>
export default {
    name: 'Profile',
    data: () => ({}),
}
</script>

<template lang="pug">
    div
        h2 User Profile

        .debug
            .hflex.hlist-3
                label.input-radio
                    input(type='radio' v-model='$s.userType' value='student')
                    span Student
                label.input-radio
                    input(type='radio' v-model='$s.userType' value='teacher')
                    span Teacher

            .div
                button(@click='doLogin') Login


        div(v-if='$s.userType == "student"')
            h3 Basic Information

            h4 Nickname
            .hcombo
                input(type='text' :value='$s.studentNick')
                button Save

            h4 Group
            .hcombo
                .select
                    select(v-model='$s.studentGroup')
                        option(v-for='group in $s.groups') {{ group.name }}
                button Save

        div(v-if='$s.userType == "teacher"')
            h2 Game-Maker Zone

            h3 My Games

            table
                tr
                    th Game
                    th Actions

                tr(v-for='game in $s.games' :key='game.id')
                    td {{ game.name }}
                    td.hcombo
                        button Edit
                        button Reset
                        button Delete
                        button(v-if='game.id != $s.game.id') Activate
                tr
                    td New game
                    td.hcombo
                        router-link(to='/edit-game')
                            button Create

            h3 My Groups
            table
                tr
                    th Name
                    th Actions
                tr(v-for='group in $s.groups')
                    td {{ group.name }}
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

table > tr > :nth-child(1)
    width 20ch
</style>

