export const SimpleState = {
    install(Vue, state) {
        Vue.util.defineReactive(Vue.prototype, '$s', state)
    }
}

export const ReactiveStorage = {
    install(Vue, options) {
        const data = {}
        const watch = {}

        for (const key of options.watch) {
            data[key] = options.storage.getItem(key)
            watch[key] = (x) => x !== null ? options.storage.setItem(key, x) : options.storage.removeItem(key)
        }

        Vue.prototype.$local = new Vue({data: data, watch: watch})
    },
}

export const SimpleApi = {
    install(Vue, options) {
        Vue.prototype.$api = async (method = 'GET', path = '', data = null) => {
            const url = options.base + path
            const login = options.getLogin()
            const token = options.getToken()

            const args = {
                method: method,
                mode: 'cors',
                credentials: 'same-origin',
                redirect: 'follow',
                referrerPolicy: 'no-referrer',
                headers: {
                    'X-Session-Login': login,
                    'X-Session-Token': token,
                },
            }
            
            if (method !== 'GET') {
                args['body'] = typeof data === 'object' && data !== null && !(data instanceof FormData) ? JSON.stringify(data) : data
            }

            return fetch(url, args).then(res => [res, null]).catch(err => [null, err])
        }
    }
}

