<script>
const dateRegex = /^([1-9][0-9]{3,})-(0?[1-9]|1[012])-(0?[1-9]|[12][0-9]|3[01])$/

export default {
    name: 'GameWizard',
    data: () => ({
        name: null,
        subtitle: null,
        deadline: null,
        resources: [
            {code: 0, name: 'Overview', format: 'Markdown'},
            {code: 1, name: 'Rules', format: 'Markdown'},
            {code: 2, name: 'Game Widget', format: 'HTML'},
        ],
        players: [
            {name: 'Judge', env: null, file: null, id: -1 },
        ],
        newPlayer: null,
        editPlayer: null,
    }),
    computed: {
        deadlineValid() {
            return String(this.deadline).match(dateRegex)
        },
    },
    methods: {
        addPlayer(player) {
            this.players.push(player)
        },
        deletePlayer(player) {
            this.players = this.players.filter(x => x.id != player.id)
        },
        patchPlayer(player) {
            for (const i in this.players) {
                if (this.players[i].id === player.id) {
                    this.players[i] = player
                    break
                }
            }
        },
        async postGame(res) {

        },
    },
    mounted() {
        if (this.$route.query.edit !== undefined) {
            console.log(`edit game ${this.$route.query.edit}`)
            // TODO: populate players with judge and refs for this game
        }
    },
}

</script>

<template lang="pug">
    div
        h2 Create Game

        h4 Basic Information
        .hflex.flex-wrap.hlist-1
            div
                h5 Name
                input(type='text' placeholder='Game name')
            div
                h5 Subtitle
                input(type='text' placeholder='Game subtitle')
            div
                h5 Deadline
                input(type='text' v-model='deadline' placeholder='2021-01-28' :class='{invalid: deadline && !deadlineValid}')

        h4 Game Resources

        table.resource-table
            tr
                th Name
                th Format
                th Actions

            tr(v-for='res in resources')
                td {{res.name}}
                td {{res.format}}
                td
                    label.input-file
                        input(type='file' :accept='res.ext' @change='res.file = $event.target.file[0]')
                        span Upload


        h4 Game Code

        table.player-table
            tr
                th Name
                th Env
                th Code
                th
                    button(@click='newPlayer = {name: "New Player", file: null}' :disabled='editPlayer !== null || newPlayer !== null') + New player

            tr(v-for="player in players" :key="player.id")
                template(v-if='editPlayer !== null && editPlayer.id === player.id')
                    td
                        input(type='text' v-model='editPlayer.name' :disabled='editPlayer.id === -1') 

                    td
                        select(v-model='editPlayer.env')
                            option(:value='null' disabled) Click to select
                            option(v-for="env in $s.envs") {{ env.name }}
                    
                    td
                        label.input-file
                            input(type='file' @change='editPlayer.file = $event.target.files[0]')
                            span Upload
                        
                    td.hcombo
                        button(@click='patchPlayer(editPlayer); editPlayer = null') Save
                        button(@click='editPlayer = null') Cancel

                template(v-else)
                    td {{ player.name }}
                    td {{ player.env || 'None' }}
                    td {{ player.file !== null ? 'Ready' : (player.file === true ? 'Uploaded' : 'None') }}
                    td.hcombo
                        button(@click='editPlayer = {...player}' :disabled='editPlayer || newPlayer') Edit
                        button(v-if='player.id !== -1' @click='deletePlayer(player)' :disabled='editPlayer || newPlayer') Delete

            tr(v-if='newPlayer !== null')
                td
                    input(type='text' v-model='newPlayer.name' placeholder='New Player') 

                td
                    select(v-model='newPlayer.env')
                        option(:value='undefined' disabled) Click to select
                        option(v-for="env in $s.envs") {{ env.name }}
                
                td
                    label.input-file
                        input(type='file' @change='newPlayer.file = $event.target.files[0]')
                        span Upload
                    
                td.hcombo
                    button(@click='addPlayer(newPlayer); newPlayer = null' :disabled='!newPlayer.name || !newPlayer.env || newPlayer.file === null') Submit
                    button(@click='newPlayer = false; newPlayer = null') Cancel

        h4 Save
        button.w-100.mb-2 Save
        button.w-100 Cancel

</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.resource-table > tr
    > :nth-child(1)
        width 25ch
    > :nth-child(2)
        width 20ch


.player-table > tr
    > :nth-child(1)
        width 25ch
    > :nth-child(2)
        width 20ch
    > :nth-child(3)
        width 15ch
    
</style>

