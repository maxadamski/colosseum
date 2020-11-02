import Vue from 'vue'
import VueRouter from 'vue-router'
import App from './pages/App.vue'
import { ReactiveStorage, SimpleApi, SimpleState } from './plugins.js'

Vue.prototype.$log = console.log

Vue.use(VueRouter)

Vue.use(SimpleState, {
    tab: 'Dashboard',
})

Vue.mixin({
    computed: {
    },
})

const router = new VueRouter({
    mode: 'history',
    base: '/',
    routes: [
        { path: '/', component: () => import('./pages/Index.vue') },
        { path: '/profile', component: () => import('./pages/Profile.vue') },
        { path: '/404', component: () => import('./pages/NotFound.vue') },
        { path: '*', redirect: '/404' },
    ],
    scrollBehavior(to, from, savedPosition) {
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

