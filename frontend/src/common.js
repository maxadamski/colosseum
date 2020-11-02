export const today = () => {
    const today = new Date()
    today.setHours(0, 0, 0, 0)
    return today
}

export const now = () => {
    return new Date()
}

