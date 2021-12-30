
export function DateToTimestamp(date) {
    return new Date(date).valueOf() / 1e3;
}