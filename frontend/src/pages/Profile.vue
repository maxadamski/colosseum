<script>
export default {
    name: 'Profile',
    data: () => ({
        nick: 'Max',
        group: 'I9-2020',
        type: 'teacher',
        groups: [
            {name: 'I1-2020', id: '1'},
            {name: 'I2-2020', id: '2'},
            {name: 'I3-2020', id: '3'},
            {name: 'I4-2020', id: '4'}
        ],
        gamess: [
            {name: 'Pentago', active:true, id: '1'},
            {name: 'Chess', active:false, id: '2'},
        ],
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
                    option(v-for="group in groups" v-bind:key="groups.id") {{ group.name }}
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
                    td Naive Player
                    td.hcombo
                        button Rename
                        button Delete
                tr
                    td Smart Player
                    td.hcombo
                        button Rename
                        button Delete
                
                tr(v-for="game in games" v-bind:key="game.id")
                    td {{ game.name }}
                    td.hcombo
                        button Edit
                        button Reset
                        

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
                tr(v-for="group in groups" v-bind:key="group.id")
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
</style>
