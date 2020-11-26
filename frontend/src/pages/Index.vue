<script>
import { now, datediff } from '../common'

export default {
    name: 'Index',
    data: () => ({
        tab: 'overview',
        tabs: ['overview', 'rules', 'play', 'team', 'submit'],
        players: ['Naive Player', 'Smart Player', 'Tricky Player'],
        player: 'Naive Player',
        submitEnv: 'C++',
        execution: 'auto',
        teamName: 'Foobar',
        firstPlayer: 'you',
        teamMembers: [
            {name: 'Max', leader: true, awaiting: false, id: '1'},
            {name: 'Piotr', leader: false, awaiting: false, id: '2'},
            {name: 'Maciej', leader: false, awaiting: false, id: '3'},
            {name: 'Sławek', leader: false, awaiting: true, id: '4'},
        ],
        invitations: [
            {inviting:"Igor", team:"FizzBuzz", id: '1'},
        ],
        submissions: [
            {date:"2020-10-11 10:15", env:"Python 3", state: "Ok", score: "80%", id:1},
            {date:"2020-10-11 10:20", env:"C++", state: "buil failed", score: "n/a", id:2},
            {date:"2020-10-11 23:12", env:"C++", state: "Ok", score: "78%", id:3},
        ],
        primarySub: 1,
        game: 'Pentago',
        timeLeft: '2 months'
    }),
    computed: {
        deadlineCountdown() {
            const dt = datediff(now(), this.$s.game.deadline) 
            let res = ''
            const plural = (x) => x == 1 ? '' : 's'
            if (dt.months > 0) res += `${dt.months} month${plural(dt.months)}`
            if (dt.days > 0) res += ` ${dt.days} day` + plural(dt.days)
            if (!res) res += `${dt.hours} hour${plural(dt.hours)} and ${dt.minutes} minute${plural(dt.minutes)}`
            return res
        },
    }
}
</script>

<template lang="pug">
div
    h1.mb-0 {{ $s.game.name }}
    h4 {{ $s.game.description }}
    b {{ deadlineCountdown }} left


    nav.tabs
        button(v-for='x in tabs' @click='tab = x' :class='{selected: tab == x}') {{x}}

    div(v-if='tab == "overview"')
        div(v-html='$s.game.overview')

    div(v-if='tab == "rules"')
        div(v-html='$s.game.rules')

    div(v-if='tab == "play"')
        h3 Interactive Game

        .hflex.hlist-6
            .vflex
                h4 Player
                .select
                    select(v-model='player')
                        option(v-for='x in players') {{x}}
            .vflex
                h4 First player
                .hflex.hlist-3
                    label.input-radio
                        input(type='radio' v-model='firstPlayer' value='you')
                        span You
                    label.input-radio
                        input(type='radio' v-model='firstPlayer' value='them')
                        span Them

        .widget
            div(v-html='$s.game.widget')


    div(v-if='tab == "team"')
        h3 Team

        h4 Team Name
        .hcombo
            input(type='text' :value='teamName')
            button Save

        h4 Team Members
        table
            tr
                th User
                th Status
                th Actions

            tr(v-for="member in teamMembers" :key="member.id")
                td {{ member.name}}

                td(v-if="member.leader") Leader 
                td(v-else-if="member.awaiting") Invited
                td(v-else) Member

                td(v-if="!member.leader && !member.awaiting") 
                    button Set Leader
                td(v-else-if="member.awaiting")
                    button Cancel Invite
                td(v-else)

        h3 Invitations
        .hflex.hlist-3.fy-center
            span(v-for="invitation in invitations" :key="invitation.id") 
                b {{ invitation.inviting }} 
                span invited you to team 
                b {{ invitation.team }}
            .hcombo
                button Accept
                button Delete

        h4 Invite Member
        .hcombo
            input(type='text' placeholder='User nickname')
            button Send

    div(v-if='tab == "submit"')
        h3 New Submission

        .hflex.hlist-6
            .vflex
                h4 Player code
                label.input-file
                    input(type='file')
                    | Upload

            .vflex
                h4 Environment
                .select
                    select(v-model='submitEnv')
                        option(v-for='env in $s.envs') {{env}}

            .vflex
                h4 Execution
                .hflex.hlist-3
                    label.input-radio
                        input(type='radio' v-model='execution' value='auto')
                        span Auto
                    label.input-radio
                        input(type='radio' v-model='execution' value='make')
                        span Makefile

            .vflex
                h4 Actions
                .hflex.hlist-1
                    button Submit
                    button Cancel


        h3 My Submissions
        table.submission-table
            tr
                th #
                th Date
                th Env
                th State
                th Score
                th Actions

            tr(v-for='sub in submissions' :key='sub.id')
                td
                    span(v-if='sub.id == primarySub')
                        b.tooltip(title="This is your team's primary submission. Your final grade will depend on the performance of this submission") {{sub.id}}*
                    span(v-else) {{ sub.id }}
                td {{ sub.date }}
                td {{ sub.env }}
                td {{ sub.state }}
                td {{ sub.score }}
                td
                    button(v-if='sub.id != primarySub' @click='primarySub = sub.id') Set Primary
</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.tabs
    display flex
    width 100%
    margin 20px 0

    button
        all unset
        flex-basis 100%
        text-align center
        border none
        border-bottom 1px solid decor-color
        height 32px
        font-size 0.8em
        background white
        cursor pointer
        text-transform uppercase

    button.selected
        border 1px solid decor-color
        border-bottom none

    button:hover
        background #eee

    button:active
        background #ddd

.widget
    background gray
    width 100%
    height 500px
    margin-top u4
    hflex center center

.submission-table > tr > :nth-child(2)
    width 20ch
</style>
