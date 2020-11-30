<script>
export default {
    name: 'Profile',
    data: () => ({}),
    methods: {
        async activateGame(gameId) {
            await this.safeApi('POST', `/games/activate/${gameId}`)
            const [groupsData, groupsStatus] = await this.safeApi('GET', `/groups`)
            this.$s.groups = groupsData

            const [envsData, envsStatus] = await this.safeApi('GET', `/environments`)
            this.$s.envs = envsData

            const [gameData, gameStatus] = await this.safeApi('GET', '/games/active')
            if (gameStatus != 200) {
                console.log(`No active game! (status code ${gameStatus})`)
            } else {
                this.$s.game = gameData
                const [gameWidget, gameWidgetStatus] = await this.safeApi('GET', `/games/${gameData.id}/widget`)
                this.$s.game.widget = gameWidget.html
                const [refPlayers, refPlayersStatus] = await this.safeApi('GET', `/games/${gameData.id}/ref_submissions`)
                this.$s.refPlayers = refPlayers
            }
            const [gamesData, gamesStatus] = await this.safeApi('GET', `/games`)
            this.$s.games = gamesData
        },
        async changeStudentNickname() {
            const [teamPatch, teamPatchStatus] = await this.safeApi('PATCH', `/students/me`, JSON.stringify({nickname: this.$s.studentNick}))
            if (teamPatchStatus === 422) {
                console.log(`Nickname cannot be empty or have more than 50 characters (status code ${teamPatchStatus})`)
                const [userData, userStatus] = await this.safeApi('GET', '/students/me')
                this.$s.studentNick = userData.nickname
            }
        },
    }

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
                input(type='text' v-model='$s.studentNick')
                button(@click="changeStudentNickname") Save

            h4 Group
            .hcombo
                .select
                    select(v-model='$s.studentGroup')
                        option(v-for='group in $s.groups' :value='group.id') {{ group.name }}
                button(@click="safeApi('PATCH', `/students/me`, JSON.stringify({group_id: $s.studentGroup}))") Save

        div(v-if='$s.userType == "teacher"')
            h2 Game-Maker Zone

            h3 My Games

            table
                tr
                    th Game
                    th Actions

                tr(v-for='(game,index) in $s.games' :key='game.id')
                    td {{ game.name }}
                    td.hcombo
                        button Edit
                        button(v-if='game.id === $s.game.id' @click="activateGame(game.id)") Reset
                        button(v-if='game.id !== $s.game.id' @click="activateGame(game.id)") Activate
                        button(v-if='game.id !== $s.game.id' @click="safeApi('DELETE', `/games/${game.id}`); $delete($s.games, index)") Delete
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
                tr(v-for='(group, index) in $s.groups')
                    td {{ group.name }}
                    td.hcombo
                        button Export
                        button Rename
                        button(@click="safeApi('DELETE', `/groups/${group.id}`); $delete($s.groups, index)") Delete
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

