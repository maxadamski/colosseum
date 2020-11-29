<script>
import {now, datediff} from '../common'

export default {
    name: 'Index',
    data: () => ({
        tab: 'overview',
        student_tabs: ['overview', 'rules', 'play', 'team', 'submit'],
        teacher_tabs: ['overview', 'rules', 'play'],
        player: 'Naive Player',
        submitEnv: 'C++',
        execution: 'auto',
        firstPlayer: 'you',
        invitedMember: '',
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
    },
    methods: {
        async fetchTeam() {
            const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
            this.$s.teamId = studentTeam.id
            this.$s.teamName = studentTeam.name
            this.$s.teamLeaderId = studentTeam.leader_id

            const [studentTeamMembers, teamMembersStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/members`)
            this.$s.teamMembers = studentTeamMembers

            const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/invitations`)
            this.$s.teamInvitations = studentTeamInvitations

            const [teamSubmissions, teamSubmissionsStatus] = await this.safeApi('GET', `/teams/${this.$s.teamId}/submissions`)
            this.$s.teamSubmissions = teamSubmissions
        },
        async fetchInvited() {
            const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/invitations`)
            this.$s.teamInvitations = studentTeamInvitations
            console.log(this.$s.teamInvitations)
        }
    }
}
</script>

<template lang="pug">
    div
        h1.mb-0 {{ $s.game.name }}
        h4 {{ $s.game.description }}
        b {{ deadlineCountdown }} left


        nav.tabs(v-if='$s.userType == "teacher"')
            button(v-for='x in teacher_tabs' @click='tab = x' :class='{selected: tab == x}') {{x}}
        nav.tabs(v-else)
            button(v-for='x in student_tabs' @click='tab = x' :class='{selected: tab == x}') {{x}}

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
                            option(v-for='x in $s.refPlayers') {{x.name}}
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
                input(type='text' v-model='$s.teamName')
                button(@click="safeApi('PATCH', `/teams/${$s.teamId}`, JSON.stringify({new_name: $s.teamName}))") Save

            h4 Team Members
            table
                tr
                    th User
                    th Status
                    th(v-if="$s.studentId === $s.teamLeaderId") Actions

                tr(v-for="member in $s.teamMembers" :key="member.id")
                    td {{ member.nickname}}

                    td(v-if="member.id === $s.teamLeaderId") Leader
                    td(v-else) Member

                    td(v-if="$s.studentId === $s.teamLeaderId && member.id !== $s.studentId ")
                        button(@click="safeApi('PATCH', `/teams/${$s.teamId}/leader/${member.id}`);$s.teamLeaderId = member.id") Set Leader
                tr(v-for="(invited, index) in $s.teamInvitations" :key="invited.id")
                    td {{ invited.nickname}}

                    td Invited

                    td(v-if="$s.studentId === $s.teamLeaderId")
                        button(@click="safeApi('DELETE', `/students/${$s.teamId}/invitations/${invited.id}`); $delete($s.teamInvitations, index)") Cancel Invite

            h3 Invitations
            .hflex.hlist-3.fy-center(v-if="($s.studentInvitations).length")
                span(v-for="(invitation, index) in $s.studentInvitations" :key="invitation.id")
                    b {{ invitation.leader }}
                    span  invited you to team 
                    b {{ invitation.name }}
                    .hcombo
                        button(@click="safeApi('POST', `/students/me/invitations/${invitation.team_id}/accept`); $delete($s.studentInvitations, index); fetchTeam()") Accept
                        button(@click="safeApi('POST', `/students/me/invitations/${invitation.team_id}/decline`); $delete($s.studentInvitations, index)") Delete
            p(v-else) There are no invitations 

            h4(v-if="$s.studentId === $s.teamLeaderId") Invite Member
                .hcombo
                    input(type='text' placeholder='Student nickname', v-model="invitedMember")
                    button(@click="safeApi('POST', `/teams/${$s.teamId}/invitations/${invitedMember}`); fetchInvited()") Send

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
                            option(v-for='env in $s.envs') {{env.name}}

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

                tr(v-for='sub in $s.submissions' :key='sub.id')
                    td
                        span(v-if='sub.id == $s.primarySub')
                            b.tooltip(title="This is your team's primary submission. Your final grade will depend on the performance of this submission") {{sub.id}}*
                        span(v-else) {{ sub.id }}
                    td {{ sub.date }}
                    td {{ sub.env }}
                    td {{ sub.state }}
                    td {{ sub.score }}
                    td
                        button(v-if='sub.id != $s.primarySub' @click='$s.primarySub = sub.id') Set Primary
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
