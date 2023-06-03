const ENCRYPT_CMD_LEN = 97;

const c_original = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,./<>?;:\'\"[]\\{}|=-+_)(*&^%$#@!~` ";
const c_encrypt = "yr(GRMJ1#\'Fme]Kc3<0`{QCBa,$pDx2[h;g8_./unU|+fELqYN~}7l>=dzX?WkjTVH%@b6s9viIo4:v5Aw&O*tP!\\S^)Z\" -";

export function encryptCMD(text) {
    let encryptedText = "";
    for (let i = 0; i < text.length; i++) {
        for (let x = 0; x < ENCRYPT_CMD_LEN; x++) {
            if (text[i] === c_original[x]) {
                encryptedText += c_encrypt[x];
                break;
            }
        }
    }
    return encryptedText;
}

export function unencryptCMD(text) {
    let decryptedText = "";
    for (let i = 0; i < text.length; i++) {
        for (let x = 0; x < ENCRYPT_CMD_LEN; x++) {
            if (text[i] === c_encrypt[x]) {
                decryptedText += c_original[x];
                break;
            }
        }
    }
    return decryptedText;
}
