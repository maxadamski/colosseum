import 'regenerator-runtime/runtime'
import Vue from 'vue'
import VueRouter from 'vue-router'
import App from './pages/App.vue'
import {SimpleState, ReactiveStorage, SimpleApi} from './plugins.js'
import {unwrap} from './common.js'

const develApiUrl = 'http://localhost:8000'
const finalApiUrl = 'https://colosseum.put.poznan.pl/api'

Vue.prototype.$log = console.log

Vue.use(VueRouter)

Vue.use(SimpleState, {
    // User role:
    userType: 'student',

    //Public data:
    groups: [
        {
            "id": 1,
            "name": "Group1"
        },
        {
            "id": 2,
            "name": "Group2"
        },
        {
            "id": 3,
            "name": "Group3"
        },
    ],
    envs: [
        {
            "id": 1,
            "name": "Env1"
        },
        {
            "id": 2,
            "name": "Env2"
        }
    ],
    game: {
        id: 1,
        name: 'Placeholder Name',
        description: 'Placeholder Description',
        deadline: new Date('2020-12-08'),
        overview: '<h2>Overview of the game</h2><p>A rendered markdown document</p>',
        rules: '<h2>Rules of the game</h2><p>A rendered markdown document</p>',
        widget: 'http://localhost:8000/game/4/interactive.html'
    },
    refPlayers: [{id: 1, name: 'Player1'}, {id: 2, name: 'Player2'}, {id: 3, name: 'Player3'}],

    // Student data:

    studentGroup: 1,
    studentId: 1,
    studentNick: 'Student1Nickname',

    studentInvitations: [
        {leader: "Student2Nickname", name: "Team1", id: '1'},
    ],

    teamId: 1,
    teamName: 'Team1',
    teamLeaderId: 1,

    teamMembers: [
        {nickname: 'Max', id: '1'},
        {nickname: 'Piotr', id: '2'},
        {nickname: 'Maciej', id: '3'}
    ],

    teamInvitations: [
        {nickname: 'Sławek', id: '4'}
    ],

    teamSubmissions: [
        {date: "2020-10-11 10:15", env: "Python 3", status: "Ok", score: "80%", id: 1, primary: false},
        {date: "2020-10-11 10:20", env: "C++", status: "buil failed", score: "n/a", id: 2, primary: false},
        {date: "2020-10-11 23:12", env: "C++", status: "Ok", score: "78%", id: 3, primary: true},
    ],

    // Teacher data:
    games: [
        {name: 'Pentago', active: true, id: '1'},
        {name: 'Chess', active: false, id: '2'},
    ],
})

Vue.use(ReactiveStorage, {
    storage: window.localStorage,
    watch: ['sessionLogin', 'sessionKey', 'sessionExp'],
})

Vue.use(SimpleApi, {
    base: process.env.NODE_ENV === 'development' ? develApiUrl : finalApiUrl,
    getLogin: () => Vue.prototype.$local.sessionLogin,
    getToken: () => Vue.prototype.$local.sessionKey,
})

Vue.mixin({
    computed: {
        isAuthorized() {
            const key = this.$local.sessionKey
            const exp = this.$local.sessionExp
            return key && exp && Number(exp) < Date.now()
        },
    },
    methods: {
        async safeApi(method, path, data) {
            const [resp, err1] = await this.$api(method, path, data)
            if (err1) return [null, -1] // network error (bad url, server down etc.)
            const [json, err2] = await unwrap(resp.json())
            if (err2) return [null, resp.status] // response body is not JSON (plain text, HTML etc.)
            return [json, resp.status]
        },
        async doLogin() {
            const login = 'student3'
            const password = 'pwd'
            const [data, status] = await this.safeApi('POST', '/login', {login: login, password: password})
            if (status != 200) {
                console.log(`login failed! (status code ${status})`)
                return
            }
            console.log(`login successful! ${data}`)
            this.$local.sessionLogin = login
            this.$local.sessionKey = data.key
            this.$local.sessionExp = data.exp
            this.$s.userType = data.is_teacher ? 'teacher' : 'student'
        },
        async fetchPublic() {
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
        },
        async fetchProfile() {
            if (this.$s.userType === "student") {
                const [userData, userStatus] = await this.safeApi('GET', '/students/me')
                this.$s.studentId = userData.id
                this.$s.studentNick = userData.nickname
                this.$s.studentGroup = userData.group_id

                const [studentInvitations, studentInvitationsStatus] = await this.safeApi('GET', '/students/me/invitations')
                this.$s.studentInvitations = studentInvitations

                const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
                this.$s.teamId = studentTeam.id
                this.$s.teamName = studentTeam.name
                this.$s.teamLeaderId = studentTeam.leader_id

                const [studentTeamMembers, teamMembersStatus] = await this.safeApi('GET', `/team/${studentTeam.id}/members`)
                this.$s.teamMembers = studentTeamMembers

                const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${studentTeam.id}/invitations`)
                this.$s.teamInvitations = studentTeamInvitations

                const [teamSubmissions, teamSubmissionsStatus] = await this.safeApi('GET', `/teams/${studentTeam.id}/submissions`)
                this.$s.teamSubmissions = teamSubmissions
            } else {
                const [gamesData, gamesStatus] = await this.safeApi('GET', `/games`)
                this.$s.games = gamesData
            }
        }
    },
})

const router = new VueRouter({
    mode: 'history',
    base: '/',
    routes: [
        {path: '/', component: () => import('./pages/Index.vue')},
        {path: '/profile', component: () => import('./pages/Profile.vue')},
        {path: '/edit-game', component: () => import('./pages/EditGame.vue')},
        {path: '/help', component: () => import('./pages/Help.vue')},
        {path: '/404', component: () => import('./pages/NotFound.vue')},
        {path: '*', redirect: '/404'},
    ],
    scrollBehavior(to, from, saved) {
        return {x: 0, y: 0}
    },
})

new Vue({
    el: '#app',
    router: router,
    render: f => f(App),
    async created() {
        await this.fetchPublic()
        if (this.isAuthorized) await this.fetchProfile()
    }
})


