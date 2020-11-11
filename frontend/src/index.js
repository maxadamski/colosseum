import 'regenerator-runtime/runtime'
import Vue from 'vue'
import VueRouter from 'vue-router'
import App from './pages/App.vue'
import { SimpleState, ReactiveStorage, SimpleApi } from './plugins.js'

const develApiUrl = 'https://colosseum.put.poznan.pl'
const finalApiUrl = 'http://localhost:8000'

Vue.prototype.$log = console.log

Vue.use(VueRouter)

Vue.use(SimpleState, {
    tab: 'Dashboard',
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
    created() {
    },
})

