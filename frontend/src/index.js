import 'regenerator-runtime/runtime'
import Vue from 'vue'
import VueRouter from 'vue-router'
import App from './pages/App.vue'
import { SimpleState, ReactiveStorage, SimpleApi } from './plugins.js'
import { unwrap } from './common.js'

const develApiUrl = 'http://localhost:8000'
const finalApiUrl = 'https://colosseum.put.poznan.pl/api'

Vue.prototype.$log = console.log

Vue.use(VueRouter)

Vue.use(SimpleState, {
    tab: 'Dashboard',
    envs: ['C++', 'C11', 'Python3'],
    groups: ['I1-2020', 'I2-2020', 'I3-2020', 'I4-2020'],
    userGroup: 'I2-2020',
    userType: 'student',
    userNick: 'Max',
    game: {
        name: 'Pentago',
        description: 'Win in a two-player match',
        deadline: new Date('2020-12-08'),
        overview: '<h2>Overview of the game</h2><p>A rendered markdown document</p>',
        rules: '<h2>Rules of the game</h2><p>A rendered markdown document</p>',
        widget: 'http://localhost:8000/game/4/interactive.html'
    },
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
            const login = 'dummy@user'
            const [data, status] = await this.safeApi('POST', '/login', {login: login, password: 'p@ssw0rd'})
            if (status != 200) {
                console.log(`login failed! (status code ${status})`)
                return
            }
            console.log(`login successful! ${data}`)
            this.$local.sessionLogin = login
            this.$local.sessionKey = data.key
            this.$local.sessionExp = data.exp
            this.$s.userType = data.isAdmin ? 'teacher' : 'student'
        },
        async fetchProfile() {
            const [data, status] = await this.safeApi('GET', '/users/me')
            // FIXME: make as few API calls as possible - include class name & others in the response.
            // Otherwise use `this.safeApi('GET', '/classes')` and set class name to the one with matching id
            // In my opinion, it's better to do most of the data processing in Python, as JS is more error prone
            this.$s.userNick = data.nickname 
        },
        async fetchPublic() {
            // I don't know id of the current game as it depends on the admin session 
        }
    },
})

const router = new VueRouter({
    mode: 'history',
    base: '/',
    routes: [
        { path: '/', component: () => import('./pages/Index.vue') },
        { path: '/profile', component: () => import('./pages/Profile.vue') },
        { path: '/edit-game', component: () => import('./pages/EditGame.vue') },
        { path: '/help', component: () => import('./pages/Help.vue') },
        { path: '/404', component: () => import('./pages/NotFound.vue') },
        { path: '*', redirect: '/404' },
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
    },
})

